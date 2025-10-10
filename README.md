# BrawlBlaze (Nome não oficial)

**Descrição:** BrawlBlaze é um jogo 3 vs 3 no estilo MOBA de Captura a Bandeira.

## Sumário:
1. [Instalação](#instalação)
2. [Compilação](#compilação)
3. [Execução](#execução)
4. [Detalhes do Jogo](#detalhes-do-jogo)
5. [Detalhes da Implementação](#detalhes-da-implementação)

---

## Instalação:

### Clonagem: ```https://github.com/artdevz/BrawlBlaze.git```

### Dependências: **g++** p/ C++ 17 e **raylib v5** p/ ClientSide

---

## Compilação:

Compilação Client: ```make client```
Compilação Server: ```make server```
Compilação Client & Server (Geral): ```make```

---

## Execução:

Execução Server: ```./bin/server/Brawlblaze```<br>
Execução Client: ```./bin/client/Brawlblaze```

---

## Detalhes do Jogo:

- **Categoria:** MOBA: 3 vs 3, 2D
- **Sobre:** "No primeiro minuto inicial cada Jogador esconde sua bandeira, tendo um total de 3 bandeiras por time. <br>
No restante os times batalharão pela Arena para capturar a bandeira inimiga e levar para a base aliada e defender suas bandeiras. <br>
O Time que conseguir capturar as 3 bandeiras primeiro vence. <br>
O tempo de respawn é de aproximadamente 15 segundos. <br>
Extra: Um Boss elemental irá nascer aos 5:00, sendo Terra: (+Armadura), Fogo(+Dano), Ar(+Velocidade), Água(+Regeneração). <br> 
O time que derrotar o boss (último a da dano) irá receber esses powerups. <br>

---

## Detalhes da Implementação:
- **Paradigma:** ECS (Entity-Component-Systems)
- **Arquitetura:** Client-Server (Servidor Autoritário)
- **Linguagem:** C++ 17
- **Engine:** Raylib 5 Input/Tela/Renderização (Client-Side)
