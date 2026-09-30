document.getElementById("preparar").addEventListener("click", async () => {
  const whey = Number(document.getElementById("whey").value);
  const agua = Number(document.getElementById("agua").value);
  const status = document.getElementById("status");

  status.textContent = "Enviando receita...";

  try {
    const resposta = await fetch("http://localhost:5090/api/receitas", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        wheyGramas: whey,
        aguaMl: agua
      })
    });

    const resultado = await resposta.json();

    if (!resposta.ok) {
      status.textContent = resultado.mensagem;
      return;
    }

    status.textContent =
      `${resultado.mensagem} Whey: ${resultado.wheyGramas} g; água: ${resultado.aguaMl} mL.`;
  } catch (erro) {
    status.textContent =
      "Não foi possível acessar a API. Confira se ela está rodando.";
    console.error(erro);
  }
}); 