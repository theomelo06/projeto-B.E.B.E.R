using BEBER.Api;
using BEBER.Api.Classes;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddCors(options =>
{
    options.AddPolicy("FrontLocal", policy =>
    {
        policy
            .WithOrigins("http://127.0.0.1:5500", "http://localhost:5500")
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});

var app = builder.Build();

app.UseCors("FrontLocal");

app.MapPost("/api/receita-personalizada", (ReceitaRequest request) =>
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

    return Results.Ok(new
    {
        mensagem = "Receita recebida pela API.",
        wheyGramas = receita.WheyGramas,
        aguaMl = receita.AguaMl,
        leiteNinhoGramas = receita.LeiteNinhoGramas,
        Sabor = receita.Sabor
    });
});

app.Run();

public record ReceitaRequest(int WheyGramas, int AguaMl, int LeiteNinhoGramas, Sabores Sabor);