const API_URL = "http://localhost:5090/api/receita-personalizada";

const formulario = document.getElementById("receitaForm");
const botao = document.getElementById("preparar");
const status = document.getElementById("status");

function mostrarStatus(mensagem, tipo = "") {
  status.textContent = mensagem;
  status.dataset.tipo = tipo;
}

formulario.addEventListener("submit", async (evento) => {
  evento.preventDefault();

  if (botao.disabled || !formulario.reportValidity()) {
    return;
  }

  const receita = {
    wheyGramas: document.getElementById("whey").valueAsNumber,
    aguaMl: document.getElementById("agua").valueAsNumber,
    leiteNinhoGramas: document.getElementById("leiteNinho").valueAsNumber,
    sabor: Number(formulario.querySelector('input[name="sabor"]:checked').value)
  };

  botao.disabled = true;
  botao.textContent = "Enviando receita...";
  mostrarStatus("Aguardando confirmação da API.");

  try {
    const resposta = await fetch(API_URL, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(receita)
    });

    // Algumas respostas de erro podem não ter um corpo JSON.
    const resultado = await resposta.json().catch(() => null);

    if (!resposta.ok) {
      throw new Error(
        resultado?.mensagem ??
        `A API recusou a solicitação (HTTP ${resposta.status}).`
      );
    }

    mostrarStatus(
      resultado?.mensagem ?? "Receita recebida pela API.",
      "sucesso"
    );
  } catch (erro) {
    console.error(erro);

    mostrarStatus(
      erro instanceof TypeError
        ? "Não foi possível conectar à API. Confira se ela está rodando."
        : erro.message,
      "erro"
    );
  } finally {
    botao.disabled = false;
    botao.textContent = "Preparar bebida →";
  }
});