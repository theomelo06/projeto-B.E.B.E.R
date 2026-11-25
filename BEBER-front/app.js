const formulario = document.getElementById("receitaForm");
const botao = document.getElementById("preparar");
const status = document.getElementById("status");
const liberar = document.getElementById("liberarPedido");

const agua = document.getElementById("agua");
const whey = document.getElementById("whey");
const leite = document.getElementById("leiteNinho");

const seletor = document.getElementById("predefinicao");
const resumo = document.getElementById("resumoReceita");

let receitas = [];
let online = false;
let ocupada = true;
let enviando = false;
let geracao = 0;
let mensagemLocal = "";

let incerto = sessionStorage.getItem("beber.pedidoIncerto");

function mostrar(mensagem, tipo = "") {
  status.textContent = mensagem;
  status.dataset.tipo = tipo;
}

function controles() {
  botao.disabled =
    enviando || !online || ocupada || Boolean(incerto);

  botao.textContent = enviando
    ? "Enviando..."
    : !online
      ? "Máquina indisponível"
      : incerto
        ? "Verificando pedido..."
        : ocupada
          ? "Preparo em andamento..."
          : "Preparar bebida →";

  liberar.hidden = !(
    incerto && online && !ocupada && !enviando
  );
}

function limparIncerto() {
  incerto = null;
  sessionStorage.removeItem("beber.pedidoIncerto");
}

function limites() {
  if (Number(seletor.value) !== 0) return;

  const volume = agua.valueAsNumber;

  if (
    Number.isInteger(volume) &&
    volume >= 100 &&
    volume <= 500
  ) {
    whey.max = Math.min(30, Math.floor(volume / 10));
    leite.max = Math.floor(volume / 5);
  }
}

// Funciona também ao acessar o site por HTTP no IP local.
function gerarId() {
  const bytes = crypto.getRandomValues(new Uint8Array(16));

  bytes[6] = (bytes[6] & 15) | 64;
  bytes[8] = (bytes[8] & 63) | 128;

  const texto = [...bytes]
    .map(numero => numero.toString(16).padStart(2, "0"))
    .join("");

  return [
    texto.slice(0, 8),
    texto.slice(8, 12),
    texto.slice(12, 16),
    texto.slice(16, 20),
    texto.slice(20)
  ].join("-");
}

function aplicarEstado(estado) {
  if (typeof estado.ocupada !== "boolean") {
    throw new Error("Status incompatível da máquina.");
  }

  online = true;
  ocupada = estado.ocupada;

  if (incerto && estado.pedidoId === incerto) {
    limparIncerto();
    mensagemLocal = "";
  }

  if (incerto) {
    mostrar(
      "Não foi possível confirmar o último pedido. Confira a máquina antes de liberar outro envio.",
      "erro"
    );
  } else if (mensagemLocal && !ocupada) {
    mostrar(mensagemLocal, "erro");
  } else {
    const origem = estado.origem === "botao"
      ? " Pedido feito no botão físico."
      : "";

    mostrar(
      (estado.mensagem ?? estado.estado) + origem,
      estado.estado === "erro"
        ? "erro"
        : estado.estado === "finalizado"
          ? "sucesso"
          : ""
    );
  }

  controles();
}

async function consultar() {
  if (enviando) return;

  const geracaoConsulta = geracao;
  const controller = new AbortController();

  const timer = setTimeout(
    () => controller.abort(),
    7000
  );

  try {
    const resposta = await fetch("/api/maquina/status", {
      cache: "no-store",
      signal: controller.signal
    });

    const resultado = await resposta.json();

    if (!resposta.ok) {
      throw new Error(
        resultado.mensagem ?? "Falha ao consultar a máquina."
      );
    }

    // Ignora consultas iniciadas antes de um novo envio.
    if (!enviando && geracaoConsulta === geracao) {
      aplicarEstado(resultado);
    }
  } catch (erro) {
    if (!enviando && geracaoConsulta === geracao) {
      online = false;

      mostrar(
        erro.name === "AbortError"
          ? "Sem resposta. Estado da máquina desconhecido."
          : erro.message,
        "erro"
      );

      controles();
    }
  } finally {
    clearTimeout(timer);
  }
}

async function acompanhar() {
  await consultar();
  setTimeout(acompanhar, 1000);
}

function selecionarReceita() {
  const id = Number(seletor.value);
  const receita = receitas.find(item => item.id === id);

  const campos = [
    agua,
    whey,
    leite,
    ...formulario.querySelectorAll('input[name="sabor"]')
  ];

  campos.forEach(campo => {
    campo.disabled = id !== 0;
  });

  if (receita) {
    agua.value = receita.aguaMl;
    whey.value = receita.wheyGramas;
    leite.value = receita.leiteNinhoGramas;

    formulario.querySelector(
      `input[name="sabor"][value="${receita.sabor}"]`
    ).checked = true;

    resumo.textContent =
      `${receita.nome}: ${receita.wheyGramas} g de whey, ` +
      `${receita.leiteNinhoGramas} g de Ninho e ` +
      `${receita.aguaMl} mL de água. ` +
      `Mesma receita do botão ${receita.id}.`;
  } else {
    resumo.textContent =
      "Ajuste os ingredientes e confira os limites da receita personalizada.";

    limites();
  }

  mensagemLocal = "";
}

async function carregarReceitas() {
  try {
    const resposta = await fetch("/api/receitas-predefinidas");

    if (!resposta.ok) {
      throw new Error("Falha ao carregar receitas predefinidas.");
    }

    receitas = await resposta.json();

    if (!Array.isArray(receitas)) {
      throw new Error("Catálogo de receitas inválido.");
    }

    for (const receita of receitas) {
      const option = document.createElement("option");

      option.value = receita.id;
      option.textContent = receita.nome;

      seletor.append(option);
    }
  } catch (erro) {
    resumo.textContent =
      `${erro.message} Recarregue a página para tentar novamente.`;
  }
}

formulario.addEventListener("submit", async evento => {
  evento.preventDefault();
  limites();

  if (botao.disabled || !formulario.reportValidity()) {
    return;
  }

  const predefinicao = Number(seletor.value);
  const pedidoId = gerarId();

  const url = predefinicao === 0
    ? "/api/receita-personalizada"
    : `/api/receitas-predefinidas/${predefinicao}/pedidos`;

  const pedido = predefinicao === 0
    ? {
        pedidoId,
        wheyGramas: whey.valueAsNumber,
        aguaMl: agua.valueAsNumber,
        leiteNinhoGramas: leite.valueAsNumber,
        sabor: Number(
          formulario.querySelector(
            'input[name="sabor"]:checked'
          ).value
        )
      }
    : { pedidoId };

  geracao++;
  enviando = true;
  mensagemLocal = "";
  incerto = pedidoId;

  sessionStorage.setItem("beber.pedidoIncerto", pedidoId);

  controles();
  mostrar("Enviando pedido...");

  const controller = new AbortController();

  const timer = setTimeout(
    () => controller.abort(),
    12000
  );

  try {
    const resposta = await fetch(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(pedido),
      signal: controller.signal
    });

    const resultado = await resposta.json().catch(() => null);

    if (!resposta.ok) {
      if (
        resposta.status === 400 ||
        resposta.status === 409 ||
        resultado?.podeTerSidoAceito === false
      ) {
        limparIncerto();
      }

      throw new Error(
        resultado?.mensagem ??
        `Erro HTTP ${resposta.status}.`
      );
    }

    if (resultado?.pedidoId !== pedidoId) {
      throw new Error("Confirmação incompatível com o pedido.");
    }

    aplicarEstado(resultado);
  } catch (erro) {
    mensagemLocal = erro.name === "AbortError"
      ? "Envio sem confirmação. Não reenvie: verificando a máquina."
      : erro.message;

    mostrar(mensagemLocal, "erro");
  } finally {
    clearTimeout(timer);
    enviando = false;
    controles();
  }
});

liberar.addEventListener("click", () => {
  if (!online || ocupada || enviando) return;

  const confirmou = confirm(
    "Você conferiu fisicamente a máquina e confirma que não há preparo em andamento?"
  );

  if (confirmou) {
    limparIncerto();
    mensagemLocal = "";
    controles();
  }
});

agua.addEventListener("input", limites);
seletor.addEventListener("change", selecionarReceita);

formulario.addEventListener("input", () => {
  mensagemLocal = "";
});

limites();
controles();
carregarReceitas();
acompanhar();