// CAROLINA MAGAGNIN WAJNER E SOFIA LEITAO

#include "raylib.h" //sofia
//#include "/opt/homebrew/Cellar/raylib/5.0/include/raylib.h" // MACBOOK carol
#include <stdio.h>
#include <string.h>

//================= CONSTANTES
#define TUBOS 4
#define WIDTH 1200
#define HEIGHT 800
#define TAMANHONOME 10
#define MAXPLAYER 5
#define FACIL 10
#define NORMAL 20
#define DIFICIL 30

//================= FUNCOES TERNARIAS

#define MINIMO(A, B) (((A)<(B)) ? (A):(B))
#define MAXIMO(A, B) (((A)>(B)) ? (A):(B))
#define check(x) ((x) > 0) ? (x - 1) : (TUBOS - 1)


//====== MENU
typedef enum Menu { INICIO = 0, GAMEPLAY = 1, DIFICULDADE = 2, GAMEOVER = 3, RANKING = 4} Menu; //Criando um enum para usar no switch case do menu

//========= STRUCTS
/////////////////////////////
typedef struct Jogador
{
    char nome[50];
    int pontos;
} Player;
//////////////////////////////
typedef struct Dificuldade
{
    int score_threshold;
    int gap;
    int inc_gap;
    int dif_max_altura;
    int inc_dif_max_altura;
    int vel_obstaculos;
    int inc_vel_obstaculos;
} Dificuldade;
/////////////////////////////
typedef struct Porco
{
    Vector2 posicao;
    int tamanho;
    float velocidade;
} Porco;

/////////////////////////////
typedef struct Tubo
{
    int auxilio, largura, comprimento,
        espaco, distancia, velocidade;
} Tubo;

////////////////////////////
typedef struct Tubos
{
    Rectangle tubogenerico;
} Tubos;
////////////////////////////
typedef struct Power
{
    Rectangle ret;
    int ativo;
} Power;
////////////////////////////
typedef struct Pontuacao
{
    int pontuacao;
    bool checagem;
} Pontuacao;

//======== VARIAVEIS GLOBAIS

//Declarando as structs
Tubos cima[TUBOS] = {0};
Tubos baixo[TUBOS] = {0};
Tubo tubo;
Porco porco;
Pontuacao pontuacao;
Dificuldade dificuldade;
Player porcos[5];
Player new_porco;
Sound som_gameover;
Sound som_pulo;
Power powerup;
Sound som_power;

Texture2D spriteporcobaixo;
Texture2D background;
Texture2D spritetuboscima;
Texture2D spritetubosbaixo;

int selecionardificuldade = NORMAL; // INICIALIZA DIFICULDADE
Color corpower = PINK;

Menu menu = INICIO; // INICIALIZA MENU

//========== DECLARACAO DE FUNCOES
void CarregarRanking(Player porcos[5]);
void AtualizarRanking(void);
void ExibirRanking(Player jogadores[5]);
void DesenhaRanking(void);
void MenuDificuldade(void);
void MudarDificuldade(int pontuacao);
void ArquivoDificuldade(char* ArquivoDificuldade);
void IniciarVariaveis(void);
void Gameplay(void);
void Inicio(void);
void Gameover(void);
void DesenharTudo(void);
void FazNovoRanking(void);
void SomPulo (void);
void SomFim (void);
void SomPower (void);
void PowerUp (int pontuacao);

//====== MAIN

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "FlappyOinc");
    SetTargetFPS(60);
    InitAudioDevice();

    Image tubobaixo = LoadImage("./sprite/tubosdebaixo.png");
    ImageResize(&tubobaixo, 100, 900);
    Image tubocima = LoadImage("./sprite/tubodecima.png");
    ImageResize(&tubocima, 100, 900);
    Image porcosprite = LoadImage("./sprite/spriteporco.gif");
    ImageResize(&porcosprite, 60, 60);


    spriteporcobaixo = LoadTextureFromImage(porcosprite);
    spritetubosbaixo = LoadTextureFromImage(tubobaixo);
    spritetuboscima = LoadTextureFromImage(tubocima);

    background = LoadTexture("./sprite/background.png");

    UnloadImage(tubobaixo);
    UnloadImage(tubocima);


    // LOOP DE TELAS

    while(!WindowShouldClose())
    {
        switch(menu) //menu
        {
            case INICIO: //tela do menu
            {
                Inicio();

            }  break;

            case GAMEPLAY: // jogo
            {
                if(selecionardificuldade == FACIL)
                {
                    ArquivoDificuldade("dificuldade0.txt");
                }

                if(selecionardificuldade == NORMAL)
                {
                    ArquivoDificuldade("dificuldade1.txt");
                }

                if(selecionardificuldade == DIFICIL)
                {
                    ArquivoDificuldade("dificuldade2.txt");
                }

                Gameplay(); // chamando a funcao gameplay

            } break;

            case DIFICULDADE: // tela das dificuldades
            {
                MenuDificuldade();

            }  break;

            case GAMEOVER: // tela de gameover
            {
                Gameover();
            }  break;

            case RANKING: // tela ranking
            {
                DesenhaRanking();
            }  break;

            default:
            {
                break;
            }
        }
    }

    CloseWindow();

    return 0;
}

//=== GAMEPLAY: Estabelece movimentacao do porco, tubos e teletransporte dos tubos

void Gameplay(void)
{
    IniciarVariaveis();

    //inicio do loop do jogo
    while(menu == GAMEPLAY && !WindowShouldClose()) // enquanto a tela eh o jogo e janela esta aberta
    {
        PowerUp(pontuacao.pontuacao);

        if(IsKeyDown(KEY_SPACE)) // se a tecla espaco eh pressionada, porco se move para cima
        {
            porco.velocidade = -8;
            SomPulo(); // faz barulho
        }

        porco.posicao.y += porco.velocidade; // posicao do porco no eixo y recebendo -8, fazendo o passaro subir (nas resolucoes de tela o y= 0 È no topo)
        porco.velocidade += 0.5; //velocidade do passaro recebendo + 0.5, consequentemente o passaro em y tbm vai receber fazendo ele cair quando o espaco nao estiver apertado

        for(int i = 0; i < TUBOS; i++)
        {
            //faz os tubos se moverem pra esquerda (direcao do player)
            baixo[i].tubogenerico.x -= dificuldade.vel_obstaculos;; //diferente do eixo y, aqui no eixo x a velocidade È somada, pq o x = 0 È na esquerda mesmo
            cima[i].tubogenerico.x -= dificuldade.vel_obstaculos;;
        }

        //faz um tubo ir pra o fim da fila dos tubos no momento em que ele sai da tela
        for(int i = 0; i < TUBOS; i++)
        {
            if(baixo[i].tubogenerico.x && cima[i].tubogenerico.x <= -tubo.largura)
            {
                baixo[i].tubogenerico.x = tubo.distancia * TUBOS - tubo.largura;
                cima[i].tubogenerico.x = tubo.distancia * TUBOS - tubo.largura;


                // estabelece espaco entre tubo de cima e de baixo
                baixo[i].tubogenerico.y = GetRandomValue(MAXIMO(50 + dificuldade.gap, baixo[check(i)].tubogenerico.y - dificuldade.dif_max_altura), MINIMO(750 - dificuldade.gap, baixo[check(i)].tubogenerico.y + dificuldade.dif_max_altura));
                cima[i].tubogenerico.y = baixo[i].tubogenerico.y - tubo.comprimento - dificuldade.gap;

                // pontuacao aumenta cada vez que tubo se teleporta
                pontuacao.pontuacao =  pontuacao.pontuacao + 1;

                // dificuldade muda de acordo com pontuacao
                MudarDificuldade(pontuacao.pontuacao);

                // porco recebe pontuacao
                new_porco.pontos = pontuacao.pontuacao;

                // para acabar o powerup
                powerup.ativo--;

                if(new_porco.pontos > porcos[4].pontos) pontuacao.checagem = true;

            }
        }

        //checa se o porco bateu nos tubos e se powerup esta ativo
        for (int i = 0; i < TUBOS; i++)
        {
            if (CheckCollisionCircleRec(porco.posicao, porco.tamanho, cima[i].tubogenerico) && powerup.ativo <= 0)
            {
                    menu = GAMEOVER; // gameover
                    AtualizarRanking();
                    SomFim(); // toca barulho gameover
            }

            else if (CheckCollisionCircleRec(porco.posicao, porco.tamanho, baixo[i].tubogenerico)  && powerup.ativo <= 0)
            {

                    menu = GAMEOVER; // gameover
                    AtualizarRanking();
                    SomFim(); // toca barulho gameover
            }
        }

        DesenharTudo(); //chamando a funcao que desenha tudo isso na tela
    }
    menu = GAMEOVER; //caso alguma das condicoes que fazem o loop mudar sejam mudadas, jogo volta pro menu
}

//======== FUNCAO SOMPULO: Estabelece o som do pulo

void SomPulo(void)
{
    som_pulo = LoadSound("./sons/pulo.mp3");

    if(IsSoundPlaying(som_pulo))
    {
        StopSound(som_pulo); // so toca uma vez
    }
    PlaySound(som_pulo);
}

//========FUNCAO SOMFIM: Estabelece o som do gameover

void SomFim (void)
{
    som_gameover = LoadSound("./sons/gameover.mp3");

    if(IsSoundPlaying(som_gameover))
    {
        StopSound(som_gameover); // so toca uma vez
    }
    PlaySound(som_gameover);
}

//========FUNCAO SOMPOWER: Estabelece o som do powerup
void SomPower (void)
{
    som_power = LoadSound("./sons/powerup.mp3");

    if(IsSoundPlaying(som_power))
    {
        StopSound(som_power); // so toca uma vez
    }
    PlaySound(som_power);
}

//======= FUNCAO POWERUP:  Estabelece condicoes para o funcionamento do powerup
void PowerUp (int pontuacao)
{

    for (int i = 0; i < TUBOS; i++)
    {
        if (pontuacao > 0 && pontuacao % 11 == 0) // a cada 11 pontos power up aparece
        {
            powerup.ret.x = 600;
            powerup.ret.y = 300;
            corpower = GOLD;

            if (CheckCollisionCircleRec(porco.posicao, porco.tamanho, powerup.ret)) // se porco tocou no powerup
            {
                powerup.ativo = 10;
                corpower = PINK;
                powerup.ret.x =  0;
                powerup.ret.y = 800;
                SomPower();
            }

        }

        else if(pontuacao % 11 < 4)
        {
            powerup.ret.x -= 1; // move powerup
            corpower = GOLD;

            if (CheckCollisionCircleRec(porco.posicao, porco.tamanho, powerup.ret)) // se porco tocou no powerup
            {
                powerup.ativo = 8; // seta tempo de duracao do powerup
                // faz quadrado desaparecer
                corpower = PINK;
                powerup.ret.x =  0;
                powerup.ret.y = 800;
                SomPower();
            }
        }

        else // faz quadrado desaparecer
        {
            corpower = PINK;
            powerup.ret.x =  0;
            powerup.ret.y = 800;
        }
    }

}

//========FUNCAO INICIO: Muda telas de acordo com teclas
void Inicio(void)
{
            DesenharTudo();// desenhando a tela do menu


            if (IsKeyPressed(KEY_D)) // caso a tecla D for pressionada, menu = dificuldade e muda pra tela de selecionar a dificuldade
            {
                menu = DIFICULDADE;
            }

            if (IsKeyPressed(KEY_G)) // caso a tecla G for pressionada, menu = gameplay e inicia o jogo
            {
                menu = GAMEPLAY;
            }

            if (IsKeyPressed(KEY_R)) // caso tecla bblabla raking
            {
                menu = RANKING;
            }
}

//========FUNCAO MENUDIFICULDADE: Faz tela de dificuldades
void MenuDificuldade(void)
{
    while(menu == DIFICULDADE)
    {
        DesenharTudo(); //funcao draw pra desenhar ela na tela

        if(IsKeyDown(KEY_E)) // caso a tecla E seja pressionada, modo easy selecionado
        {
            selecionardificuldade = FACIL;
            menu = INICIO;
        }

        if(IsKeyDown(KEY_N)) // caso a tecla N seja pressionada, modo normal selecionado
        {
            selecionardificuldade = NORMAL;
            menu = INICIO;
        }

        if(IsKeyDown(KEY_H)) // caso a tecla H seja pressionada, modo hard selecionado
        {
            selecionardificuldade = DIFICIL;
            menu = INICIO;
        }

        if(IsKeyDown(KEY_SPACE))
        {
            menu = INICIO;
        }
    }
}


//========FUNCAO MUDARDIFICULDADE :  De acordo com os indices, muda a dificuldade do jogo
void MudarDificuldade(int pontuacao)
{
    if (pontuacao % dificuldade.score_threshold == 0) // a cada tantos pontos
    {
        // Ajustar variaveis de dificuldade
        dificuldade.gap -= dificuldade.inc_gap;
        dificuldade.dif_max_altura += dificuldade.inc_dif_max_altura;
        dificuldade.vel_obstaculos += dificuldade.inc_vel_obstaculos;
    }
}

//========FUNCAO ARQUIVODIFICULDADE: Abre e le os arquivos de dificuldades, de acordo com a solicitada
void ArquivoDificuldade(char* ArquivoDificuldade)
{
    FILE *arquivo = fopen(ArquivoDificuldade, "r"); // abre arquivo pedido

    if (arquivo != NULL)
    {
        fscanf(arquivo, "%d %d %d %d %d %d %d", &dificuldade.score_threshold, &dificuldade.gap, &dificuldade.inc_gap,
            &dificuldade.dif_max_altura, &dificuldade.inc_dif_max_altura, &dificuldade.vel_obstaculos, &dificuldade.inc_vel_obstaculos); // escaneia
        fclose(arquivo);
    }

    else
    {
        menu = GAMEOVER;
    }

}

//========FUNCAO GAMEOVER : Cria tela gameover
//menu do gameover
void Gameover(void)
{
        DesenharTudo();

        if(IsKeyPressed(KEY_SPACE))
        {
            menu = INICIO;
        }

        if(IsKeyPressed(KEY_G))
        {
            menu = GAMEPLAY;
        }
}

//========FUNCAO CARREGARRANKING: Abre e le arquivo ranking
void CarregarRanking(Player porcos[5])
{
    FILE *arquivoRanking = fopen("ranking.bin", "rb");

    if(arquivoRanking == NULL) // se arquivo nao existe
    {
        FazNovoRanking();
    }

    else
    {
        fread(porcos, sizeof(Player), 5, arquivoRanking);
        fclose(arquivoRanking);
    }
}

//========FUNCAO FAZNOVORANKING:   Faz arquivo bin de ranking
void FazNovoRanking(void)
{
    Player porcosf[MAXPLAYER];
    FILE *arq;

    for(int i = 0; i < MAXPLAYER; i++) // escreve 5 jogadores com 0 npontos
    {
        strcpy(porcosf[i].nome, TextFormat("Oinc %i", i));
        porcosf[i].pontos = 0;
    }

    arq = fopen("ranking.bin", "wb");

    if (arq != NULL) // salva no arquivo
    {
        fwrite(porcosf, sizeof(Player), MAXPLAYER, arq);
        fclose(arq);
    }

}


//========FUNCAO DESENHARANKING: Faz tela de ranking
void DesenhaRanking(void)
{
    DesenharTudo();
    CarregarRanking(porcos);

    if(IsKeyPressed(KEY_SPACE))
    {
        menu = INICIO;
    }
}

//========FUNCAO ATUALIZARRANKING:  Adiciona Jogador ao ranking
void AtualizarRanking(void)
{
    int letterCount = 0;
    int j, k;

        if (new_porco.pontos > porcos[4].pontos) // se pontuacao for maior do que a do ultimo do ranking
        {
            int key = GetCharPressed();

            while(!WindowShouldClose() && !IsKeyPressed(KEY_ENTER)) // desenhar tela que pede nome do jogador
            {
                DesenharTudo();
                key = GetCharPressed();

                if ((key >= 97) && (key <= 122) && (letterCount < TAMANHONOME)) // letras aparecem na tela e string eh lida
                {
                    new_porco.nome[letterCount] = (char)key;
                    new_porco.nome[letterCount+1] = '\0';
                    letterCount++;
                }

                if (IsKeyPressed(KEY_BACKSPACE)) // caso usuario apague
                {
                    letterCount--;
                    if (letterCount < 0) letterCount = 0;
                    new_porco.nome[letterCount] = '\0';
                }
            }

            for(j = 0; j < 5; j++) // enquanto nao passar por todos os jogadores
            {
                if(new_porco.pontos > porcos[j].pontos) // se a pontuacao do jogador for maior do cont do ranking
                {

                    for(k = MAXPLAYER - 1; k > j-1; k--) // colocar o restante dos jogadores uma posicao abaixo no ranking
                    {
                        strcpy(porcos[k].nome, porcos[k-1].nome);
                        porcos[k].pontos = porcos[k-1].pontos;

                    }

                    strcpy(porcos[j].nome, new_porco.nome); // colocar o nome do jogador no nome do ranking
                    porcos[j].pontos = new_porco.pontos; // colocar a pontuacao do jogador na pontuacao do ranking

                    j = MAXPLAYER;
                }
            }

            FILE *arquivoRanking = fopen("ranking.bin", "wb");

            if(arquivoRanking != NULL)
            {
                fwrite(porcos, sizeof(Player), 5, arquivoRanking); // salva no arquivo
                fclose(arquivoRanking);
            }

        }
}

//========FUNCAO INICIARVARIAVEIS: Inicializa Variaveis
void IniciarVariaveis(void)
{
    //inicializando o score
    pontuacao.pontuacao = 0;
    pontuacao.checagem = false;

    //inicializando as variaveis do jogador
    porco.posicao = (Vector2){300, 300};
    porco.tamanho = 20;
    porco.velocidade = 1;

    new_porco.pontos = 0;

     //inicializando todas as dimensoes que envolvem os tubos
    tubo.auxilio = 0;
    tubo.velocidade = dificuldade.vel_obstaculos;
    tubo.largura = 100;
    tubo.comprimento = HEIGHT;
    tubo.espaco = dificuldade.gap; //+ dificuldade.gap; //+ dificuldade.gap para aumenter/diminuir o espaco entre o tubo de cima e de baixo
    tubo.distancia = 450; //+ dificuldade.range;// + dificuldade.range para aumentar/diminuir o espacamento entre os tubos
    baixo[3].tubogenerico.y = GetRandomValue(50 + dificuldade.gap, 750 - dificuldade.gap);
    CarregarRanking(porcos);

    powerup.ret.x =  0;
    powerup.ret.y = 800;
    powerup.ativo = 0;

    //depois de inicializar todas as dimensoes dos tubos, incializao das posicoes inicias (4 tubos em cima e em baixo)
    for(int i = 0; i < TUBOS; i++)
    {
        if(i > 0)
        {
            baixo[i].tubogenerico.y = GetRandomValue(MAXIMO(50 + dificuldade.gap, baixo[check(i)].tubogenerico.y - dificuldade.dif_max_altura), MINIMO(750 - dificuldade.gap, baixo[check(i)].tubogenerico.y + dificuldade.dif_max_altura));
            cima[i].tubogenerico.y = baixo[i].tubogenerico.y - tubo.comprimento - dificuldade.gap;
        }

        else
        {
            baixo[0].tubogenerico.y = GetRandomValue(MAXIMO(50 + dificuldade.gap, baixo[3].tubogenerico.y - dificuldade.dif_max_altura), MINIMO(750 - dificuldade.gap, baixo[3].tubogenerico.y + dificuldade.dif_max_altura));
            cima[0].tubogenerico.y = baixo[3].tubogenerico.y - tubo.comprimento - dificuldade.gap;
        }

        baixo[i].tubogenerico.width = tubo.largura;
        baixo[i].tubogenerico.height = tubo.comprimento;

        baixo[i].tubogenerico.x = WIDTH - tubo.largura + tubo.auxilio;
        cima[i].tubogenerico.x = WIDTH - tubo.largura + tubo.auxilio;

        cima[i].tubogenerico.width = tubo.largura;
        cima[i].tubogenerico.height = tubo.comprimento;

        tubo.auxilio += tubo.distancia;
    }
}

//========FUNCAO DESENHARTUDO: Desenha todo o jogo
void DesenharTudo(void)
{
    BeginDrawing(); //iniciando o draw, isso aqui È obrigatorio

    switch(menu)
    {
        case INICIO://desenhando o INICIO(o menu)
        {
            // fundo do jogo
            DrawTexture(background, 0, 0, WHITE);;

            DrawText("FLAPPY OINC", 20, 20, 50, WHITE);

            DrawRectangle(400, 300, 300, 50, WHITE);
            DrawText("JOGAR (g)", 445, 310, 35, PINK);

            DrawRectangle(400, 400, 300, 50, WHITE);
            DrawText("DIFICULDADE (d)", 407, 412, 33, PINK);

            DrawRectangle(400, 500, 300, 50, WHITE);
            DrawText("Ranking (r)", 457, 512, 35, PINK);

            DrawRectangle(400, 600, 300, 50, WHITE);
            DrawText("SAIR (esc)", 450, 610, 35, PINK);


        }   break;

        case GAMEPLAY://desenhando a gameplay
        {


            DrawTexture(background, 0, 0, WHITE); // fundo do jogo

            for(int i = 0; i < TUBOS; i++)
            {

                DrawTexture(spritetuboscima, cima[i].tubogenerico.x, cima[i].tubogenerico.y, WHITE);
                DrawTexture(spritetubosbaixo, baixo[i].tubogenerico.x, baixo[i].tubogenerico.y, WHITE);

            }

            DrawTexture(spriteporcobaixo, porco.posicao.x - 20, porco.posicao.y - 20, WHITE);
            DrawText(TextFormat("pontos: %i", pontuacao.pontuacao),0,0, 40, WHITE);


            DrawRectangle(powerup.ret.x, powerup.ret.y, 20, 20, corpower);

            if(powerup.ativo > 0)
            {
                DrawText("VOCE ESTA INVENCIVEL", 400,0, 40, GOLD);
            }

        }   break;

        case DIFICULDADE: //desenhando o menu da dificuldade
        {
            DrawTexture(background, 0, 0, WHITE);

            DrawRectangle(220, 170, 800, 50, WHITE);
            DrawText("PRESSIONE ESPACO PARA VOLTAR",250, 180, 40, PINK);

            DrawRectangle(500, 500, 200, 50, WHITE);
            DrawText("EASY (e)", 520, 510, 35, PINK);

            DrawRectangle(500, 560, 200, 50, WHITE);
            DrawText("NORMAL (n)", 513, 572, 30, PINK);

            DrawRectangle(500, 620, 200, 50, WHITE);
            DrawText("HARD (h)", 520, 630, 35, PINK);

        } break;

        case GAMEOVER: // desenhando o menu do gameover
            {
                if(pontuacao.checagem == true)
                {
                    DrawTexture(background, 0, 0, WHITE);

                    DrawText("Entrou no ranking!!", 450, 250, 35, WHITE);

                    DrawText("Nickname em minusculas: ", 210, 350, 40, WHITE);
                    DrawText(new_porco.nome, 400, 450, 40, WHITE);
                    int sairdatela;

                    if(IsKeyDown(KEY_ENTER))
                    {
                        sairdatela = 1;
                    }

                    if(sairdatela == 1)
                    {
                        DrawText("pressione -g- pra jogar novamente", 450, 400, 15, WHITE);
                        DrawText("pressione -espaco- pra voltar ao menu", 450, 430, 15, WHITE);
                    }

                }

                if(pontuacao.checagem == false)
                {
                    DrawTexture(background, 0, 0, WHITE);

                    DrawText("Game Over", 450, 350, 35, WHITE);

                    DrawText("pressione -g- pra jogar novamente", 450, 400, 20, WHITE);
                    DrawText("pressione -espaco- pra voltar ao menu", 450, 430, 20, WHITE);
                }

            } break;

            case RANKING: // tela ranking
            {
               // DrawRectangle(0, 0, 1200, 800, PINK);
                DrawTexture(background, 0, 0, WHITE);

                    DrawText("RANKING", 20, 20, 50, WHITE);

                    DrawRectangle(450, 300 , 350, 50, WHITE);
                    DrawText(TextFormat("%s: %i pontos",porcos[0].nome, porcos[0].pontos), 470, 310, 35, PINK);

                    DrawRectangle(450, 360 , 350, 50, WHITE);
                    DrawText(TextFormat("%s: %i pontos",porcos[1].nome, porcos[1].pontos), 470, 370, 35, PINK);

                    DrawRectangle(450, 420 , 350, 50, WHITE);
                    DrawText(TextFormat("%s: %i pontos",porcos[2].nome, porcos[2].pontos), 470, 430, 35, PINK);

                    DrawRectangle(450, 480 , 350, 50, WHITE);
                    DrawText(TextFormat("%s: %i pontos",porcos[3].nome, porcos[3].pontos), 470, 490, 35, PINK);

                    DrawRectangle(450, 540 , 350, 50, WHITE);
                    DrawText(TextFormat("%s: %i pontos",porcos[4].nome, porcos[4].pontos), 470, 550, 35, PINK);

            }  break;

        default:
        {
            break;
        }
    }

    EndDrawing(); //finalizando o draw
}
