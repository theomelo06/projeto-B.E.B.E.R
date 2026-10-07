namespace BEBER.Api.Classes
{
    public class ReceitaPersonalizadaRequest
    {
        public ReceitaPersonalizadaRequest(int WheyGramas, int AguaMl, int LeiteNinhoGramas, Sabores Sabor)
        {
            this.WheyGramas = WheyGramas;
            this.AguaMl = AguaMl;
            this.LeiteNinhoGramas = LeiteNinhoGramas;
            this.Sabor = Sabor;
        }

        public int WheyGramas;
        public int AguaMl;
        public int LeiteNinhoGramas;
        public Sabores Sabor;

        public void Confere()
        {
            if (AguaMl < 100 || AguaMl > 500)
            {
                throw new ArgumentException(
                    "A quantidade de água deve estar entre 100 e 500 mL."
                );
            }

            if (WheyGramas < 0 || LeiteNinhoGramas < 0)
            {
                throw new ArgumentException(
                    "As quantidades dos pós não podem ser negativas."
                );
            }

            if (WheyGramas > 30)
            {
                throw new ArgumentException(
                    "O valor máximo de whey é de 30 gramas."
                );
            }

            if (WheyGramas * 10L > AguaMl)
            {
                throw new ArgumentException(
                    "Use no máximo 10 g de whey para cada 100 mL de água."
                );
            }

            if (LeiteNinhoGramas * 5L > AguaMl)
            {
                throw new ArgumentException(
                    "Use no máximo 20 g de Leite Ninho para cada 100 mL de água."
                );
            }

            if (!Enum.IsDefined(typeof(Sabores), Sabor))
            {
                throw new ArgumentException("Escolha um sabor válido.");
            }
        }



    }

}
