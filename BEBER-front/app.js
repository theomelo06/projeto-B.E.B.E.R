document
  .getElementById("preparar")
  .addEventListener("click", () => {
    const whey = Number(document.getElementById("whey").value);
    const agua = Number(document.getElementById("agua").value);

    document.getElementById("status").textContent =
      `Receita selecionada: ${whey} g de whey e ${agua} mL de água.`;
  });