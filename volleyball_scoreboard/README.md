# App de Placar de Vôlei (Arduino + Touchscreen)

Este sketch implementa um placar de vôlei para **dois times** em **tela horizontal**, com visual claro e controle por toque.

## Recursos

- Pontuação por toque (`+A`, `-A`, `+B`, `-B`).
- Regra de set:
  - vence ao chegar em **21 pontos**, desde que tenha **2 pontos de vantagem**;
  - em `20x20`, continua até abrir 2 de diferença.
- Sistema de **melhor de 3 sets** (vence quem fizer 2 sets).
- Botão **Reset** para reiniciar toda a partida.

## Dependências

- Biblioteca [`TFT_eSPI`](https://github.com/Bodmer/TFT_eSPI)
- Display touch compatível com `getTouch()` da `TFT_eSPI`.

## Como usar

1. Copie a pasta `volleyball_scoreboard` para sua pasta de sketches.
2. Abra `volleyball_scoreboard.ino` na Arduino IDE.
3. Configure `TFT_eSPI/User_Setup.h` para seu display/touch.
4. Compile e envie para a placa.

## Observação

Se seu touch usa outra biblioteca/controlador, você pode adaptar apenas a leitura de toque na função `loop()` mantendo toda a lógica de sets/pontos.
