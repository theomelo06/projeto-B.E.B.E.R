namespace BEBER.Api.Classes;

public record ReceitaPredefinida(
    int Id,
    string Nome,
    int WheyGramas,
    int AguaMl,
    int LeiteNinhoGramas,
    int Sabor
);

public static class ReceitasPredefinidas
{
    public static readonly IReadOnlyList<ReceitaPredefinida> Todas =
        new ReceitaPredefinida[]
        {
            new(1, "Smooth Chocolate", 30, 300, 25, 1),
            new(2, "Smooth Strawberry", 30, 300, 25, 2),
            new(3, "Smooth Vanilla", 30, 300, 25, 3),
            new(4, "Smooth Cookies", 30, 300, 25, 4),
            new(5, "Sweet Mix", 30, 300, 25, 5),
            new(6, "Power Dose", 30, 220, 0, 3)
        };
}