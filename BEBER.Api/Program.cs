using BEBER.Api;
using BEBER.Api.Classes;
using Microsoft.Extensions.FileProviders;
using System.Net.Http.Json;
using System.Text;
using System.Text.Json;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddCors(options =>
{
    options.AddPolicy("FrontLocal", policy =>
    {
        policy
            .WithOrigins(
                "http://127.0.0.1:5500",
                "http://localhost:5500"
            )
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});

builder.Services.AddHttpClient("Esp32", client =>
{
    var endereco = builder.Configuration["Esp32:BaseUrl"]
        ?? throw new InvalidOperationException(
            "Configure Esp32:BaseUrl no appsettings.json."
        );

    client.BaseAddress = new Uri(endereco);
    client.Timeout = TimeSpan.FromSeconds(5);
});

var app = builder.Build();

var pastaFront = Path.GetFullPath(
    Path.Combine(
        app.Environment.ContentRootPath,
        "..",
        "BEBER-front"
    )
);

app.UseFileServer(new FileServerOptions
{
    FileProvider = new PhysicalFileProvider(pastaFront),
    EnableDefaultFiles = true,
    EnableDirectoryBrowsing = false
});

app.UseCors("FrontLocal");

// Confirma apenas que a API está funcionando.
app.MapGet("/api/status", () => Results.Ok(new
{
    mensagem = "BEBER API está funcionando."
}));

app.MapGet("/api/receitas-predefinidas", () =>
    Results.Ok(ReceitasPredefinidas.Todas)
);

// Consulta a situação real da ESP.
app.MapGet(
    "/api/maquina/status",
    async (IHttpClientFactory factory) =>
    {
        try
        {
            var estado = await Consultar(
                factory.CreateClient("Esp32")
            );

            return Results.Ok(estado);
        }
        catch (TaskCanceledException)
        {
            return Falha(
                504,
                "A ESP não respondeu a tempo. Estado desconhecido."
            );
        }
        catch (HttpRequestException)
        {
            return Falha(
                503,
                "ESP indisponível. Confira a rede e o IP."
            );
        }
        catch (JsonException)
        {
            return Falha(
                502,
                "Status incompatível. Confira o firmware da ESP."
            );
        }
    }
);

app.MapPost(
    "/api/receita-personalizada",
    async (
        ReceitaRequest request,
        IHttpClientFactory factory
    ) =>
    {
        var receita = new ReceitaPersonalizadaRequest(
            request.WheyGramas,
            request.AguaMl,
            request.LeiteNinhoGramas,
            request.Sabor
        );

        try
        {
            receita.Confere();
        }
        catch (ArgumentException erro)
        {
            return Falha(400, erro.Message);
        }

        if (!Guid.TryParseExact(
            request.PedidoId,
            "D",
            out var id
        ))
        {
            return Falha(400, "Identificador de pedido inválido.");
        }

        var pedido = new
        {
            pedidoId = id.ToString(),
            wheyGramas = receita.WheyGramas,
            aguaMl = receita.AguaMl,
            leiteNinhoGramas = receita.LeiteNinhoGramas,
            sabor = (int)receita.Sabor
        };

        return await Enviar(
            factory.CreateClient("Esp32"),
            pedido,
            id.ToString()
        );
    }
);

app.MapPost(
    "/api/receitas-predefinidas/{receitaId:int}/pedidos",
    async (
        int receitaId,
        PedidoPredefinido request,
        IHttpClientFactory factory
    ) =>
    {
        if (!ReceitasPredefinidas.Todas.Any(
            receita => receita.Id == receitaId
        ))
        {
            return Falha(400, "Receita predefinida inexistente.");
        }

        if (!Guid.TryParseExact(
            request.PedidoId,
            "D",
            out var id
        ))
        {
            return Falha(400, "Identificador de pedido inválido.");
        }

        // A ESP usa as quantidades da rotina do botão.
        var pedido = new
        {
            pedidoId = id.ToString(),
            predefinicao = receitaId
        };

        return await Enviar(
            factory.CreateClient("Esp32"),
            pedido,
            id.ToString()
        );
    }
);

app.Run();

static IResult Falha(
    int codigo,
    string mensagem,
    bool podeTerSidoAceito = false
)
{
    return Results.Json(
        new { mensagem, podeTerSidoAceito },
        statusCode: codigo
    );
}

static bool EstadoValido(EstadoMaquina? estado)
{
    return estado?.Ocupada is not null &&
        estado.Estado is
            "ocioso" or
            "recebido" or
            "preparando" or
            "finalizado" or
            "erro";
}

static async Task<EstadoMaquina> Consultar(HttpClient cliente)
{
    using var resposta = await cliente.GetAsync("api/status");

    resposta.EnsureSuccessStatusCode();

    var estado = await resposta.Content
        .ReadFromJsonAsync<EstadoMaquina>();

    if (!EstadoValido(estado))
    {
        throw new JsonException("Status incompatível.");
    }

    return estado!;
}

static async Task<IResult> Enviar(
    HttpClient cliente,
    object pedido,
    string id
)
{
    bool iniciouEnvio = false;

    try
    {
        var atual = await Consultar(cliente);

        if (atual.Ocupada == true && atual.PedidoId != id)
        {
            return Falha(409, "A máquina já está ocupada.");
        }

        using var conteudo = new StringContent(
            JsonSerializer.Serialize(pedido),
            Encoding.UTF8,
            "application/json"
        );

        iniciouEnvio = true;

        using var resposta = await cliente.PostAsync(
            "api/pedidos",
            conteudo
        );

        var texto = await resposta.Content.ReadAsStringAsync();

        var options = new JsonSerializerOptions(
            JsonSerializerDefaults.Web
        );

        if (!resposta.IsSuccessStatusCode)
        {
            var erro = JsonSerializer.Deserialize<ErroEsp>(
                texto,
                options
            );

            int codigo = (int)resposta.StatusCode;

            return Falha(
                codigo is 400 or 409 ? codigo : 502,
                erro?.Mensagem ?? "A ESP recusou o pedido.",
                codigo is not (400 or 409)
            );
        }

        var retorno = JsonSerializer.Deserialize<EstadoMaquina>(
            texto,
            options
        );

        if (!EstadoValido(retorno) || retorno!.PedidoId != id)
        {
            return Falha(
                502,
                "Confirmação inesperada. Confira a máquina antes de reenviar.",
                true
            );
        }

        return Results.Json(
            retorno,
            statusCode: (int)resposta.StatusCode
        );
    }
    catch (TaskCanceledException)
    {
        return Falha(
            504,
            "A ESP não respondeu a tempo. Consulte o estado antes de reenviar.",
            iniciouEnvio
        );
    }
    catch (HttpRequestException)
    {
        return Falha(
            503,
            "Falha de comunicação com a ESP. Confira rede e IP.",
            iniciouEnvio
        );
    }
    catch (JsonException)
    {
        return Falha(
            502,
            "Resposta incompatível da ESP.",
            iniciouEnvio
        );
    }
}

public record ReceitaRequest(
    string? PedidoId,
    int WheyGramas,
    int AguaMl,
    int LeiteNinhoGramas,
    Sabores Sabor
);

public record PedidoPredefinido(string? PedidoId);

public record EstadoMaquina(
    string? PedidoId,
    string? Origem,
    string? Estado,
    bool? Ocupada,
    string? Mensagem
);

public record ErroEsp(string? Mensagem);