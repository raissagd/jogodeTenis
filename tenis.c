#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <math.h>

const int SCREEN_W = 900; // largura tela
const int SCREEN_H = 700; // altura tela
const float FPS = 60;
ALLEGRO_FONT *FONT_42;

// variaveis globais
int bolasEmCampo = 0;
int pontosP1 = 0, pontosP2 = 0;
char placar1[10], placar2[10];
int vitoriasp1 = 0, vitoriasp2 = 0, ganhador;
char recorde, auxrecorde;

// define para os jogadores
#define DIST_FUNDO 50
#define ALTURA_JOGADOR 20
#define LARGURA_JOGADOR 110
#define VEL_JOGADOR 5.0

// define para as bolas
#define NUM_MIN_BOLAS 12
#define NUM_MAX_BOLAS 20
#define RAIO_MIN_BOLA 10
#define RAIO_MAX_BOLA 15
#define VEL_MAX_BOLA 3.5
#define VEL_MIN_BOLA 2.5
//----------------------- structs ----------------------------------------------------------//

// Coordenadas, para usar nas bolas
typedef struct Coordenada
{
    float x;
    float y;
} Coordenada;

typedef struct Jogador
{
    float x, y, h, w, vel;
    int dir, esq, cima, baixo;
    ALLEGRO_COLOR cor;
    int id;
    bool acao;
    bool podeRebater;
} Jogador;

typedef struct Bola
{
    Coordenada centro; // coordenadas do centro da bola
    float raio;
    Coordenada movimento; // meio que a velocidade em x e y
    ALLEGRO_COLOR color;
    int onscreen;
    int id;
} Bola;

typedef struct Canhao
{
    Coordenada _local;
    Bola bolas[NUM_MAX_BOLAS];
    int id;
} Canhao;

typedef struct paredao
{
    float x, y;
} paredao;

//---------------------------------------------------------------------------------//
/*
void initParedao(paredao *pr)
{
    pr->x = 60; // largo o suficiente para que, independentemente do angulo de saida da bola, ela passe pelo retangulo
    pr->y = 60;
}
*/
//---------------------------------------------------------------------------------//

float randFloat(float max)
{
    return (float)rand() / RAND_MAX * max;
}

int randInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int randSinal() // positivo ou negativo, para ajudar na hora da mudanca de sentido da bola
{
    if (rand() % 2)
    {
        return 1;
    }
    return -1;
}

//----------------------- funcao do canhao ----------------------------------------------------------//

void initCanhao(Canhao *c1, int _id, Coordenada local)
{
    c1->id = _id;
    c1->_local = local;
}

//----------------------- funcoes relacionadas aos jogadores ----------------------------------------------------------//
void desenhaJogador(Jogador p)
{

    al_draw_filled_rectangle(p.x, p.y, p.x + p.w, p.y + p.h, p.cor);
}

void initJogador(Jogador *p)
{
    p->h = ALTURA_JOGADOR;
    p->w = LARGURA_JOGADOR;
    p->x = SCREEN_W / 2 - p->w / 2;
    p->dir = 0;
    p->esq = 0;
    p->cima = 0;
    p->baixo = 0;
    p->vel = VEL_JOGADOR;
    p->acao = 0;
    p->podeRebater = 1;
}

void initJogador1(Jogador *p1)
{
    initJogador(p1);
    p1->y = SCREEN_H - DIST_FUNDO - DIST_FUNDO;
    p1->cor = al_map_rgb(204, 0, 246);
    p1->id = 1;
}

void initJogador2(Jogador *p2)
{
    initJogador(p2);
    p2->y = SCREEN_H - SCREEN_H + DIST_FUNDO + DIST_FUNDO + DIST_FUNDO + DIST_FUNDO + DIST_FUNDO;
    p2->cor = al_map_rgb(235, 0, 130);
    p2->id = 2;
}

// funcoes para limitar o movimento dos jogadores na quadra
void atualizaJogador1(Jogador *p) // jogador de baixo
{
    if (p->dir && p->x + p->vel <= SCREEN_W - LARGURA_JOGADOR)
        p->x += p->vel;
    else if (p->esq && p->x - p->vel >= 0)
        p->x -= p->vel;

    else if (p->baixo && p->y + p->vel <= SCREEN_H - ALTURA_JOGADOR)
        p->y += p->vel;
    else if (p->cima && p->y - p->vel >= 440) // altura da rede, pra baixo
        p->y -= p->vel;
}

void atualizaJogador2(Jogador *p) // jogador de cima
{
    if (p->dir && p->x + p->vel <= SCREEN_W - LARGURA_JOGADOR)
        p->x += p->vel;
    else if (p->esq && p->x - p->vel >= 0)
        p->x -= p->vel;

    else if (p->baixo && p->y + p->vel <= 375) // altura da rede
        p->y += p->vel;
    else if (p->cima && p->y - p->vel >= 165) // altura do placar, mais ou menos
        p->y -= p->vel;
}

//---------------------------------------------------------------------------------------------------------------//

//----------------------- funcoes relacionadas as bolinhas ----------------------------------------------------------//

void drawBola(Bola *b)
{
    al_draw_filled_circle(b->centro.x, b->centro.y, b->raio, b->color);
}

Bola CriaBolaRandom(float x, float y, Bola *b, int posicao) // = Criar bola aleatoria
{
    b->raio = randInt(RAIO_MIN_BOLA, RAIO_MAX_BOLA);                                // raio de cada bola eh aleatorio
    b->color = al_map_rgb(randInt(150, 255), randInt(150, 255), randInt(150, 255)); // cor de cada bola eh aleatoria
    b->onscreen = 1;
    b->centro.x = x;
    b->centro.y = y;
    printf("\nx = %1.f y = %1.f", b->centro.x, b->centro.y);

    switch (posicao)
    {
    case 0: // SUPERIOR DIREITO
        b->movimento.x = -(VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        b->movimento.y = (VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        // printf("SUP DIR\n");
        break;

    case 1: // INFERIOR DIREITO
        b->movimento.x = -(VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        b->movimento.y = -(VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        // printf("INF DIR\n");
        break;

    case 2: // INFERIOR ESQUERDO
        b->movimento.x = (VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        b->movimento.y = -(VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        // printf("INF ES\n");
        break;

    case 3: // SUPERIOR ESQUERDO
        b->movimento.x = (VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        b->movimento.y = (VEL_MIN_BOLA + randFloat(VEL_MAX_BOLA));
        // printf("SUP ES\n");
        break;
    }

    return *b;
}

void AdicionaBola(Canhao *c, Bola b, int posicao) // = atualiza bola
{
    if (bolasEmCampo < NUM_MAX_BOLAS)
    {
        c->bolas[posicao] = b;
        bolasEmCampo++;
    }
}

void MovimentaBola(Bola *b) // velocidade?
{
    b->centro.x += b->movimento.x;
    b->centro.y += b->movimento.y;
}

void MantemBolaNaTela(Bola *b) // = colisoes com a quadra
{
    if (b->centro.x > SCREEN_W || b->centro.x < 0) // lida com as colisoes laterais
        b->movimento.x = -b->movimento.x;

    else if (b->centro.y > SCREEN_H)
        b->onscreen = 0; // a bola saiu por baixo

    else if (b->centro.y < 0)
        b->onscreen = 0; // a bola saiu por cima
}

void bolaOnouOff(Bola *b)
{
    if (b->centro.y > 680 && b->centro.y < 684 && b->movimento.y > 0) // limites um pouco antes do fim da quadra, que coloquei para permitir
    // que eu possa contar quantas vezes a bola saiu
    {
        printf("\nBola saiu por baixo.\n");
        pontosP2++;
    }
    else if (b->centro.y < 95 && b->centro.y > 90 && b->movimento.y < 0)
    {
        printf("\nBola saiu por cima\n");
        pontosP1++;
    }
}

//----------------------- funcoes relacionadas a jogadores & bolas ----------------------------------------------------------//

void colisaojogadorbola(Bola *b, Jogador *p1, Jogador *p2)
{
    if (b->centro.x <= p1->x + LARGURA_JOGADOR && b->centro.x >= p1->x && b->centro.y < p1->y && b->centro.y + b->raio >= p1->y - ALTURA_JOGADOR && p1->acao)
    {
        float antigox, antigoy;
        antigox = b->movimento.x;
        antigoy = b->movimento.y;

        b->movimento.x = 0;                     // para a bola, para mudar a direcao do movimento
        b->movimento.x = antigox * randSinal(); // vai ser aleatorio se a bola vai voltar da esquerda ou da direita

        if (b->movimento.y > 0)
        {
            b->movimento.y = -b->movimento.y; // mudaDirecaoY
        }
    }

    else if (b->centro.x <= p2->x + LARGURA_JOGADOR && b->centro.x >= p2->x && b->centro.y > p2->y + ALTURA_JOGADOR && b->centro.y + b->raio <= p2->y + (2 * ALTURA_JOGADOR) && p2->acao)
    {
        float antigox, antigoy;
        antigox = b->movimento.x;
        antigoy = b->movimento.y;

        b->movimento.x = 0;                     // para a bola, para mudar a direcao do movimento
        b->movimento.x = antigox * randSinal(); // vai ser aleatorio se a bola vai voltar da esquerda ou da direita

        if (b->movimento.y < 0)
        {
            b->movimento.y = -b->movimento.y;
        }
    }
}

void recomecaJogo()
{
    if (pontosP1 == 11) // quando bater em 10, o placar vai zerar e o jogador ja vai comecar com 0 de novo, contando 1 set
    {
        vitoriasp1++;
        pontosP1 = 0;
        pontosP2 = 0;
    }
    else if (pontosP2 == 11)
    {
        vitoriasp2++;
        pontosP1 = 0;
        pontosP2 = 0;
    }
}

void quemGanhou()
{
    if (vitoriasp1 == 0 && vitoriasp2 == 0 && pontosP2 < pontosP1)
        ganhador = 1;
    else if (vitoriasp1 == 0 && vitoriasp2 == 0 && pontosP1 < pontosP2)
        ganhador = 2;
    if (vitoriasp2 < vitoriasp1)
    {
        ganhador = 1;
        recorde = vitoriasp1;
    }
    else if (vitoriasp1 < vitoriasp2)
    {
        ganhador = 2;
        recorde = vitoriasp2;
    }
}

void mudaCor1(Jogador *p1)
{
    if (pontosP1 == 3)
        p1->cor = al_map_rgb(255, 0, 0);
}

void mudaCor2(Jogador *p2)
{
    if (pontosP2 == 3)
        p2->cor = al_map_rgb(255, 0, 0);
}

/*
void drawParedao1(paredao *pr)
{ // em cima
    al_draw_filled_rectangle(-120, -50, SCREEN_W + 120, SCREEN_H + 50, al_map_rgb(255, 255, 255));
}

void drawParedao2(paredao *pr)
{ // em baixo
    al_draw_filled_rectangle(-120, SCREEN_H + 50, SCREEN_W + 120, S//CREEN_H + 50, al_map_rgb(255, 255, 255));
}
*/

/*
void colisaoParedao(Bola *bola, paredao *pr1, paredao *pr2)
{
    int contador = 0, j;
    if (bola->centro.x >= -pr1->x && bola->centro.x <= pr1->x + SCREEN_W && bola->centro.y <= -pr1->y && bola->centro.y >= -pr1->y - 10)
    {
        // contagemCima++;
        // pontosP1++;
        contador++;
        printf("\nUma bola saiu por cima. Ponto do jogador 1.");
        // printf("\n%d\n", contagemCima);
    }
    else if (bola->centro.x <= pr2->x + SCREEN_W && bola->centro.x >= -pr2->x && bola->centro.y <= pr2->y + SCREEN_H + 10 && bola->centro.y >= pr2->y + SCREEN_H)
    {
        // contagemBaixo++;
        // pontosP2++;
        printf("\nUma bola saiu por baixo. Ponto do jogador 2.");
        // printf("\n%d\n", contagemBaixo);
    }
}
*/

/* nao consegui achar o por que de ela nao estar funcionando
int newRecord(int recorde, int auxrecorde)
{
    FILE *arq = fopen("recorde.txt", "r");
    auxrecorde = -1;
    if (arq != NULL)
    {
        fscanf(arq, "%d", recorde);
        fclose(arq);
    }
    if (auxrecorde < recorde)
    {
        arq = fopen("recorde.txt", "w");
        fprintf(arq, "%d", recorde);
        fclose(arq);
        return 1;
    }
    return 0;
}

*/

/*------------------- MAIN ----------------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{

    int i, j;
    int vezCanhao = 0;
    int contador = 0;

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    ALLEGRO_BITMAP *imagem = NULL;
    ALLEGRO_SAMPLE *music = NULL;

    //----------------------- rotinas de inicializacao ----------------------------------------------------------//

    // inicializa o Allegro
    if (!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }

    // inicializa o módulo de primitivas do Allegro
    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

    // inicializa o modulo que permite carregar imagens no jogo
    if (!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize image module!\n");
        return -1;
    }

    // cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if (!timer)
    {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }

    // cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        return -1;
    }

    // instala o teclado
    if (!al_install_keyboard())
    {
        fprintf(stderr, "failed to install keyboard!\n");
        return -1;
    }

    // inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();

    // inicializa o modulo allegro que entende arquivos tff de fontes
    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "failed to load tff font module!\n");
        return -1;
    }

    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);

    // cria a musica de fundo
    music = al_load_sample("song.ogg");
    if (!music)
    {
        fprintf(stderr, "Falha ao carregar som\n");
        return -1;
    }

    // carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *font = al_load_font("arial.ttf", 42, 1);
    if (font == NULL)
    {
        fprintf(stderr, "font file does not exist or cannot be accessed!\n");
    }

    // cria a fila de eventos
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        return -1;
    }

    // registra na fila os eventos de tela (ex: clicar no X na janela)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    // registra na fila os eventos de tempo: quando o tempo altera de t para t+1
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    // registra na fila os eventos de teclado (ex: pressionar uma tecla)
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // inicia o temporizador
    al_start_timer(timer);

    /*------------------- INIT JOGADORES -------------------------------------------------------------------------------- */
    Jogador p1;
    initJogador1(&p1);
    Jogador p2;
    initJogador2(&p2);

    /*------------------- INIT CANHOES -------------------------------------------------------------------------------- */
    /*
    canhao superior direito: 890.0 10.0
    canhao inferior esquerdo 890.0 690.0
    canhao inferior direito 10.0 690.0
    canhao superior esquerdo 10.0 10.0
    */

    Coordenada canhao0;
    canhao0.x = 890.0;
    canhao0.y = 10.0;

    Canhao SUPdir;
    initCanhao(&SUPdir, 0, canhao0);

    Coordenada canhao1;
    canhao1.x = 890.0;
    canhao1.y = 690.0;

    Canhao INFdir;
    initCanhao(&INFdir, 1, canhao1);

    Coordenada canhao2;
    canhao2.x = 10.0;
    canhao2.y = 690.0;

    Canhao INFes;
    initCanhao(&INFes, 2, canhao2);

    Coordenada canhao3;
    canhao0.x = 10.0;
    canhao0.y = 10.0;

    Canhao SUPes;
    initCanhao(&SUPes, 3, canhao3);

    /*------------------- INIT BOLAS E PAREDOES -------------------------------------------------------------------------------- */

    Bola b;

    FONT_42 = al_load_font("arial.ttf", 32, 1);
    // Carregar a imagem
    imagem = al_load_bitmap("quadrabit.bmp"); // upload da imagem

    int playing = 1;
    int tempo;

    al_play_sample(music, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

    /*------------------- COMECO DO WHILE -------------------------------------------------------------------------------- */
    while (playing)
    {
        ALLEGRO_EVENT ev;
        // espera por um evento e o armazena na variavel de evento ev
        al_wait_for_event(event_queue, &ev);

        // se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
        if (ev.type == ALLEGRO_EVENT_TIMER)
        {

            // Desenha a imagem na tela
            al_draw_bitmap(imagem, 0, 0, 0);

            // atualiza e desenha os jogadores
            atualizaJogador1(&p1);
            desenhaJogador(p1);
            atualizaJogador2(&p2);
            desenhaJogador(p2);

            // escrever
            sprintf(placar1, "%d", pontosP1);
            sprintf(placar2, "%d", pontosP2);

            al_draw_text(font, al_map_rgb(255, 255, 255), 405, 65, ALLEGRO_ALIGN_CENTRE, &placar1[0]);
            al_draw_text(font, al_map_rgb(255, 255, 255), 505, 65, ALLEGRO_ALIGN_CENTRE, &placar2[0]);

            // movimentos com a bola:

            for (int g = contador; g >= 0; g--)
            {
                MovimentaBola(&(SUPdir.bolas[g]));
                MantemBolaNaTela(&(SUPdir.bolas[g]));
                colisaojogadorbola((&(SUPdir.bolas[g])), &p1, &p2);
                // colisaoParedao(&(SUPdir.bolas[g]), &pr1, &pr2);
                bolaOnouOff(&(SUPdir.bolas[g]));
                drawBola(&(SUPdir.bolas[g]));

                MovimentaBola(&(INFdir.bolas[g]));
                MantemBolaNaTela(&(INFdir.bolas[g]));
                colisaojogadorbola((&(INFdir.bolas[g])), &p1, &p2);
                // colisaoParedao(&(INFdir.bolas[g]), &pr1, &pr2);
                bolaOnouOff(&(INFdir.bolas[g]));
                drawBola(&(INFdir.bolas[g]));

                MovimentaBola(&(INFes.bolas[g]));
                MantemBolaNaTela(&(INFes.bolas[g]));
                colisaojogadorbola((&(INFes.bolas[g])), &p1, &p2);
                // colisaoParedao(&(INFes.bolas[g]), &pr1, &pr2);
                bolaOnouOff(&(INFes.bolas[g]));
                drawBola(&(INFes.bolas[g]));

                MovimentaBola(&(SUPes.bolas[g]));
                MantemBolaNaTela(&(SUPes.bolas[g]));
                colisaojogadorbola((&(SUPes.bolas[g])), &p1, &p2);
                // colisaoParedao(&(SUPes.bolas[g]), &pr1, &pr2);
                bolaOnouOff(&(SUPes.bolas[g]));
                drawBola(&(SUPes.bolas[g]));
            }

            recomecaJogo(); // quando um jogador fizer 10 pontos, o placar eh zerado
            quemGanhou();

            mudaCor1(&p1);
            mudaCor2(&p2);

            // atualiza a tela (quando houver algo para mostrar)
            al_flip_display();
            // al_rest(10.0);

            if (al_get_timer_count(timer) % (int)FPS == 0)
            {
                tempo = (al_get_timer_count(timer) / FPS);
                // printf("\n%d segundos se passaram...", tempo);
                if (tempo % (int)5 == 0) // ou seja, de 5 em 5 segundos
                {

                    if (vezCanhao == 0)
                    {
                        b = (CriaBolaRandom(890, 10, &b, 0));
                        AdicionaBola(&SUPdir, b, contador);
                        vezCanhao++;
                    }
                    else if (vezCanhao == 1)
                    {
                        b = (CriaBolaRandom(890, 690, &b, 1));
                        AdicionaBola(&INFdir, b, contador);
                        vezCanhao++;
                        // printf("INF DIR\n");
                    }
                    else if (vezCanhao == 2)
                    {
                        b = (CriaBolaRandom(10, 690, &b, 2));
                        AdicionaBola(&INFes, b, contador);
                        vezCanhao++;
                        // printf("INF ES\n");
                    }
                    else if (vezCanhao == 3)
                    {
                        b = (CriaBolaRandom(10, 10, &b, 3));
                        AdicionaBola(&SUPes, b, contador);
                        vezCanhao = 0;
                        contador++;
                        // printf("SUP ES\n");
                    }

                    printf("\n%d bolas Em Campo", bolasEmCampo);
                }
            }
        }
        // clicar no x da tela para mostrar a tela final com o recorde
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            char my_text[30];
            // colore toda a tela de preto
            al_clear_to_color(al_map_rgb(0, 0, 0));

            /*
            tentei fazer com que o recorde fosse mostrado, mas, por algum motivo, a funcao nao funciona
            if (newRecord(recorde, auxrecorde))
            {
                al_draw_text(font, al_map_rgb(200, 20, 30), SCREEN_W / 3, 100 + SCREEN_H / 2, 0, "NEW RECORD!");
            }
            else
            {
                sprintf(my_text, "Record: %d", recorde);
                al_draw_text(font, al_map_rgb(0, 200, 30), SCREEN_W / 3, 100 + SCREEN_H / 2, 0, my_text);
            }
            */
            sprintf(my_text, "O jogador %d ganhou!", ganhador);
            al_draw_text(font, al_map_rgb(0, 200, 30), SCREEN_W / 2, SCREEN_H / 2 - 80, ALLEGRO_ALIGN_CENTRE, my_text);

            sprintf(my_text, "Placar de %d x %d", pontosP1, pontosP2);
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 30, ALLEGRO_ALIGN_CENTRE, my_text);

            sprintf(my_text, "Sets jogados: %d x %d", vitoriasp1, vitoriasp2);
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTRE, my_text);

            // reinicializa a tela, espera 3 segundos e fecha
            al_flip_display();
            al_rest(7);
            playing = 0;
        }
        // se o tipo de evento for um pressionar de uma tecla
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_W:
                p1.cima = 1;
                break;

            case ALLEGRO_KEY_S:
                p1.baixo = 1;
                break;

            case ALLEGRO_KEY_A:
                p1.esq = 1;
                break;

            case ALLEGRO_KEY_D:
                p1.dir = 1;
                break;
            case ALLEGRO_KEY_UP:
                p2.cima = 1;
                break;

            case ALLEGRO_KEY_DOWN:
                p2.baixo = 1;
                break;

            case ALLEGRO_KEY_LEFT:
                p2.esq = 1;
                break;

            case ALLEGRO_KEY_RIGHT:
                p2.dir = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                if (p1.podeRebater)
                {
                    p1.acao = 1;
                    // printf("personagem 1 executou uma acao\n");
                    p1.podeRebater = 0;
                }

                else
                {
                    p1.acao = 0;
                }

                break;

            case ALLEGRO_KEY_ENTER:
                if (p2.podeRebater)
                {
                    p2.acao = 1;
                    // printf("personagem 2 executou uma acao\n");
                }
                else
                {
                    p2.acao = 0;
                }

                break;
            }

            // imprime qual tecla foi
            // printf("\ncodigo tecla: %d", ev.keyboard.keycode);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {

            switch (ev.keyboard.keycode)
            {
                // se a tecla for o W
            case ALLEGRO_KEY_W:
                p1.cima = 0;
                break;

            case ALLEGRO_KEY_S:
                p1.baixo = 0;
                break;

            case ALLEGRO_KEY_A:
                p1.esq = 0;
                break;

            case ALLEGRO_KEY_D:
                p1.dir = 0;
                break;
            case ALLEGRO_KEY_UP:
                p2.cima = 0;
                break;

            case ALLEGRO_KEY_DOWN:
                p2.baixo = 0;
                break;

            case ALLEGRO_KEY_LEFT:
                p2.esq = 0;
                break;

            case ALLEGRO_KEY_RIGHT:
                p2.dir = 0;
                break;
            case ALLEGRO_KEY_SPACE:
                p1.acao = 0;
                p1.podeRebater = 1;
                break;
            case ALLEGRO_KEY_ENTER:
                p2.acao = 0;
                p2.podeRebater = 1;
                break;
            }
        }

    } // fim do while

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagem);
    al_destroy_sample(music);

    return 0;
}