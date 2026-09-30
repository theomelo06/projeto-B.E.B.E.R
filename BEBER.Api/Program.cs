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

app.MapPost("/api/receitas", (ReceitaRequest receita) =>
{
    if (receita.WheyGramas < 0 || receita.AguaMl <= 0)
    {
        return Results.BadRequest(new
        {
            mensagem = "Informe quantidades válidas."
        });
    }

    return Results.Ok(new
    {
        mensagem = "Receita recebida pela API.",
        wheyGramas = receita.WheyGramas,
        aguaMl = receita.AguaMl
    });
});

app.Run();

record ReceitaRequest(decimal WheyGramas, int AguaMl);