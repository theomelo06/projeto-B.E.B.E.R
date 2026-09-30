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
            if(this.AguaMl > 500)
            {
                throw new ArgumentException("O valor máximo de água é de 500ml");
            }
            if(this.WheyGramas * 10 > this.AguaMl)
            {
                throw new ArgumentException("O valor máximo de whey é de 10% da quantidade de água");
            }
            if(this.WheyGramas > 30)
            {
                throw new ArgumentException("O valor máximo de whey é 30 gramas");
            }
            if(this.LeiteNinhoGramas > 0.2 * this.AguaMl)
            {
                throw new ArgumentException("O valor máximo de leite ninho é de 20% da quantidade de água");
            }
        }



    }

}
