using BEBER.Api;
using BEBER.Api.Classes;
using System.Net.Http.Json;
using System.Text.Json;
using Microsoft.Extensions.FileProviders;
using System.Text;

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
    RequestPath = "",
    EnableDefaultFiles = true,
    EnableDirectoryBrowsing = false
});

app.UseCors("FrontLocal");

app.MapGet("/api/status", () => Results.Ok(new
{
    mensagem = "BEBER API está funcionando."
}));

app.MapPost(
    "/api/receita-personalizada",
    async (
        ReceitaRequest request,
        IHttpClientFactory httpClientFactory
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
            return Results.BadRequest(new
            {
                mensagem = erro.Message
            });
        }

        var pedido = new
        {
            pedidoId = Guid.NewGuid().ToString(),
            wheyGramas = receita.WheyGramas,
            aguaMl = receita.AguaMl,
            leiteNinhoGramas = receita.LeiteNinhoGramas,
            sabor = (int)receita.Sabor
        };

        var cliente = httpClientFactory.CreateClient("Esp32");

        try
        {
            var json = JsonSerializer.Serialize(
            pedido,
            new JsonSerializerOptions(JsonSerializerDefaults.Web)
            );

            using var conteudo = new StringContent(
                json,
                Encoding.UTF8,
                "application/json"
            );

            Console.WriteLine($"Enviando para ESP: {json}");

            using var resposta = await cliente.PostAsync(
                "api/pedidos",
                conteudo
            );

            var retorno = await resposta.Content
                .ReadFromJsonAsync<EspResposta>();

            if (!resposta.IsSuccessStatusCode)
            {
                int codigo = (int)resposta.StatusCode;

                return Results.Json(
                    new
                    {
                        mensagem = retorno?.Mensagem
                            ?? "A ESP recusou o pedido."
                    },
                    statusCode: codigo is 400 or 409 ? codigo : 502
                );
            }

            if (retorno is null ||
                retorno.PedidoId != pedido.pedidoId ||
                retorno.Estado != "recebido")
            {
                return Results.Json(
                    new
                    {
                        mensagem = "A ESP retornou uma confirmação inesperada."
                    },
                    statusCode: 502
                );
            }

            return Results.Ok(new
            {
                pedidoId = retorno.PedidoId,
                estado = retorno.Estado,
                mensagem = retorno.Mensagem
            });
        }
        catch (TaskCanceledException)
        {
            return Results.Json(
                new
                {
                    pedidoId = pedido.pedidoId,
                    mensagem =
                        "A confirmação da ESP não chegou a tempo. " +
                        "Confira o Monitor Serial antes de reenviar."
                },
                statusCode: 504
            );
        }
        catch (HttpRequestException)
        {
            return Results.Json(
                new
                {
                    mensagem =
                        "Falha na comunicação com a ESP. " +
                        "Confira a conexão Wi-Fi e o endereço configurado."
                },
                statusCode: 503
            );
        }
        catch (JsonException)
        {
            return Results.Json(
                new
                {
                    mensagem = "A ESP retornou uma resposta JSON inválida."
                },
                statusCode: 502
            );
        }
    });

app.Run();

public record ReceitaRequest(
    int WheyGramas,
    int AguaMl,
    int LeiteNinhoGramas,
    Sabores Sabor
);

public record EspResposta(
    string? PedidoId,
    string? Estado,
    string? Mensagem
);