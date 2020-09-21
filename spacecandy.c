#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>

//Tamanho da tela
#define WIDTH 840
#define HEIGHT 600
//Tamanho da matriz
#define CANDYWIDTH 508
#define CANDYHEIGHT 530
//Tamanho da lateral e topo do jogo
#define MARGINLEFT 307
#define MARGINTOP 35
//Margem interna dos doces
#define MARGIN 10
#define FPS 60
//O mínimo deve ser 3 doces e o máximo 6
#define TYPES 6
#define PLAYS 8
#define N_LINES 9
#define N_COLS 9
//Distância que pode trocar os doces
#define DISTANCE 1
//O objetivo é marcar 3 ou +
#define OBJETIVO 3
//Doces
#define ANIMATION 0
#define LILAC 1
#define RED 2
#define YELLOW 3
#define BLUE 4
#define PURPLE 5
#define GREEN 6

typedef struct Candy {
	int type;
	int offset_lin;
	int offset_col;
	int active;
	ALLEGRO_COLOR cor;
} Candy;

Candy M[N_LINES][N_COLS];
ALLEGRO_COLOR colors[TYPES+1];

const float CELL_W = (float)(CANDYWIDTH)/N_COLS;
const float CELL_H = (float)(CANDYHEIGHT)/N_LINES;

int score=0, plays=PLAYS;
char my_score[100], my_plays[100];

ALLEGRO_FONT *size_f;

//Gera os doces randomicamente
int generateRandom() {
	return rand()%TYPES + 1;
}
//Gera os doces com a função generateRandom()
void initCandies() {
	int i, j;
	for(i=0; i<N_LINES; i++) {
		for(j=0; j<N_COLS; j++) {
			if(M[i][j].type == 0) {
				M[i][j].type = generateRandom();
				M[i][j].offset_col = 0;
				M[i][j].offset_lin = 0;
				M[i][j].active = 1;		
				M[i][j].cor = colors[M[i][j].type];	
			}
		}
	}
}
//Preenche a matriz com os doces
void initGame() {
	int i, j;
	for(i=0; i<N_LINES; i++) {
		for(j=0; j<N_COLS; j++) {
			M[i][j].type = generateRandom();
			M[i][j].offset_col = 0;
			M[i][j].offset_lin = 0;
			M[i][j].active = 1;
			M[i][j].cor = colors[M[i][j].type];
		}
	}
}
//Pausa para animação
void pauseAnimation(ALLEGRO_TIMER *timer) {
	al_stop_timer(timer);
	al_rest(0.5);
	al_start_timer(timer);
}
//Estilo dos doces
void drawCandy(int lin, int col) {
	int cell_x = MARGINLEFT + CELL_W*col;
	int cell_y = MARGINTOP + CELL_H*lin;
	if(M[lin][col].type == LILAC) {
		// doce no estilo de triângulo
		// al_draw_filled_triangle(cell_x+MARGIN, cell_y + CELL_H - MARGIN,
		//                         cell_x + CELL_W - MARGIN, cell_y + CELL_H - MARGIN,
		//                         cell_x + CELL_W/2, cell_y+MARGIN,
		//                         M[lin][col].cor);
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	} 
	else if(M[lin][col].type == RED) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	} 
	else if(M[lin][col].type == BLUE) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	} 	
	else if(M[lin][col].type == YELLOW) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	}
	else if(M[lin][col].type == PURPLE) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	}
	else if(M[lin][col].type == GREEN) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2,
		                       CELL_W/2-MARGIN, CELL_H/2-MARGIN,
		                       M[lin][col].cor);
	}
}
//Cenário do jogo com recorde e jogadas
void drawScenario(ALLEGRO_DISPLAY *display, int bg) {
	//Jogadas
	sprintf(my_plays, "Jogadas: %d", plays);
	if (bg == 1){
		al_draw_text(size_f, al_map_rgb(150, 60, 60), 60, (HEIGHT/2)-60, 0, my_plays);
	} 
	else if (bg == 2){
		al_draw_text(size_f, al_map_rgb(250, 223, 165), 60, (HEIGHT/2)-60, 0, my_plays);
	} 
	//Recorde
	sprintf(my_score, "Score: %d", score);
	if (bg == 1){
		al_draw_text(size_f, al_map_rgb(150, 60, 60), 95, (HEIGHT/2)+25, 0, my_score);
	} 
	else if (bg == 2){
		al_draw_text(size_f, al_map_rgb(250, 223, 165), 95, (HEIGHT/2)+25, 0, my_score);
	} 
	int i, j;
	for(i=0; i<N_LINES; i++) {
		for(j=0; j<N_COLS; j++) {
			drawCandy(i, j);
		}
	}    
}
//Apaga a sequência realizada, muda a som e cor.
int clearSequence(int li, int lf, int ci, int cf) {
	ALLEGRO_SAMPLE *effect = al_load_sample("sound/effect.ogg");
	int i, j, count=0;
	for(i=li; i<=lf; i++) {
		for(j=ci; j<=cf; j++) {
			count++;
			M[i][j].active = 0;
			//Pinta de branco
			M[i][j].cor = colors[ANIMATION];
			al_play_sample(effect, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
		}
	}
	return count;
}
//Verifica se as sequências a serem eliminadas valem ponto. Cada doce vale 1 ponto
int processaMatriz() {
	//retorna a quantidade de pontos feitos
	int i, j, k, count = 0;
	int current, seq;
	//procura na horizontal:
	for(i=0; i<N_LINES; i++) {
		current = M[i][0].type;
		seq = 1;
		for(j=1; j<N_COLS; j++) {
			if(current == M[i][j].type && current > 0) {
				seq++;
				if(j == N_COLS-1 && seq >= OBJETIVO)
					count += clearSequence(i, i, j-seq+1, N_COLS-1);
			}
			else {
				if(seq >= OBJETIVO) {
					count += clearSequence(i, i, j-seq, j-1);
				}
				seq = 1;
				current = M[i][j].type;					
			}
		}
	}
	//procura na vertical:
	for(j=0; j<N_COLS; j++) {
		current = M[0][j].type;
		seq = 1;
		for(i=1; i<N_LINES; i++) {
			if(current == M[i][j].type && current > 0) {
				seq++;
				if(i == N_LINES-1 && seq >=OBJETIVO) 
					count += clearSequence(i-seq+1, N_LINES-1, j, j);
			}
			else {
				if(seq >= OBJETIVO) {
					count += clearSequence(i-seq, i-1, j, j);
				}
				seq = 1;
				current = M[i][j].type;	
			}
		}
	}
	return count;
}
//Atualiza as linhas/colunas quando faz sequencia
void atualizaOffset() {
	int i, j, offset;
	for(j=0; j<N_COLS; j++) {
		offset = 0;
		for(i=N_LINES-1; i>=0; i--) {
			M[i][j].offset_lin = offset;
			if(M[i][j].active == 0) {
				M[i][j].type = 0;
				offset++;
				M[i][j].active = 1;
			}
			else {
				M[i][j].offset_lin = offset;
			}
		}
	}
}
//Faz os doces descerem
void atualizaMatriz() {
	int i, j, offset;
	for(j=0; j<N_COLS; j++) {
		for(i=N_LINES-1; i>=0; i--) {
			offset = M[i][j].offset_lin;
			if(offset > 0) {
				M[i+offset][j].type = M[i][j].type;
				M[i+offset][j].active = M[i][j].active;
				M[i+offset][j].cor = M[i][j].cor;
				M[i][j].type = 0;
				M[i][j].active = 0;
				M[i][j].offset_lin = 0;
			}
		}
	}
}
//Doce selecionado com o mouse
void getCell(int x, int y, int *lin, int *col) {
	x -= MARGINLEFT;
	y -= MARGINTOP;
	*lin = y/CELL_H;
	*col = x/CELL_W;
}
//Distancia dos doces
int candyDistance(int lin1, int col1, int lin2, int col2) {
	return sqrt(pow(lin1-lin2, 2) + pow(col1-col2, 2));
}
//Troca os doces
void change(int lin1, int col1, int lin2, int col2) {
	Candy aux = M[lin1][col1];
	M[lin1][col1] = M[lin2][col2];
	M[lin2][col2] = aux;
	plays--;
}
//Le o arquivo e grava o score
int newRecord(int score, int *record) {
	FILE *arq = fopen("score.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%d", record);
		fclose(arq);
	}
	if(*record < score ) {
		arq = fopen("score.txt", "w");
		fprintf(arq, "%d", score);
		fclose(arq);
		return 1;
	}
	return 0;
}
int main(){
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	//-----Início das rotinas do Allegro-----

	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}
	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
	display = al_create_display(WIDTH, HEIGHT);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}
	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");
	//Inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	//Inicializa o modulo allegro que entende arquivos tff de fontes
	al_init_ttf_addon();
	//Inicializa o modulo allegro que carrega img
	al_init_image_addon();
	if (!al_install_audio())
	{
		fprintf(stderr, "Falha ao inicializar áudio.\n");
		return false;
	}
	if (!al_init_acodec_addon())
	{
		fprintf(stderr, "Falha ao inicializar codecs de áudio.\n");
		return false;
	}
	if (!al_reserve_samples(1))
	{
		fprintf(stderr, "Falha ao alocar canais de áudio.\n");
		return false;
	}
	//Carrega o arquivo Candyshop.ttf da fonte Candyshop e define que sera usado o tamanho 32
	size_f = al_load_font("font/Candyshop.ttf", 32, 1);
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
	al_install_keyboard();
   	//Registra na fila de eventos que eu quero identificar quando a tela foi alterada
	al_register_event_source(event_queue, al_get_display_event_source(display));
   	//Registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//Registra o teclado na fila de eventos:
	al_register_event_source(event_queue, al_get_keyboard_event_source());   
	//Registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());    
   	//Inicia o temporizador
	al_start_timer(timer);

	//-----Fim do início do Allegro-----
	
	//Cores
	colors[ANIMATION] = al_map_rgb(255,255,255);
	colors[LILAC] = al_map_rgb(97, 15, 182);
	colors[RED] = al_map_rgb(183, 27, 6);
	colors[YELLOW] = al_map_rgb(255, 220, 0);
	colors[BLUE] = al_map_rgb(20, 224, 255);
	colors[PURPLE] = al_map_rgb(134, 41, 107);
	colors[GREEN] = al_map_rgb(20, 255, 102);

	//Música Menu
	ALLEGRO_SAMPLE *bgmusic = al_load_sample("sound/audio3.ogg");
	ALLEGRO_SAMPLE_INSTANCE *menuMusic = al_create_sample_instance(bgmusic);
	al_set_sample_instance_playmode(menuMusic, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(menuMusic, al_get_default_mixer());

	//Música LVL 1
	ALLEGRO_SAMPLE *lvl1 = al_load_sample("sound/audio.ogg");
	ALLEGRO_SAMPLE_INSTANCE *lvl1Audio = al_create_sample_instance(lvl1);
	al_set_sample_instance_playmode(lvl1Audio, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(lvl1Audio, al_get_default_mixer());

	//Música LVL 2
	ALLEGRO_SAMPLE *lvl2 = al_load_sample("sound/audio2.ogg");
	ALLEGRO_SAMPLE_INSTANCE *lvl2Audio = al_create_sample_instance(lvl2);
	al_set_sample_instance_playmode(lvl2Audio, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(lvl2Audio, al_get_default_mixer());
	
	//Posições dos doces
	srand(2);
	
	initGame();
	int n_zeros = processaMatriz();
	while(n_zeros > 0) {
		do {
			atualizaOffset();
			atualizaMatriz();
		} while(processaMatriz());
		initCandies();
		n_zeros = processaMatriz();
	} 

	ALLEGRO_BITMAP *bgmenu = al_load_bitmap("img/bgmenu2.bmp");
	ALLEGRO_BITMAP *background1 = al_load_bitmap("img/bg.bmp");
	ALLEGRO_BITMAP *background2 = al_load_bitmap("img/bg2.bmp");
	ALLEGRO_BITMAP *gameover = al_load_bitmap("img/bggo.bmp");

	int pontos, start = 1, menu = 1, playing = 1, end = 1, choose = 1, col_src, lin_src, col_dst, lin_dst, flag_animation = 0;

	while (start){
		while(menu){
			al_stop_sample_instance(lvl1Audio);
			al_stop_sample_instance(lvl2Audio);
			pontos = 0; 
			plays = PLAYS;
			playing = 1;
			score = 0;
			end = 1;
			al_play_sample_instance(menuMusic);
			ALLEGRO_EVENT evm;
			al_draw_bitmap(bgmenu,0,0,0);
			al_flip_display();
			al_wait_for_event(event_queue, &evm);
			if(evm.type == ALLEGRO_EVENT_KEY_DOWN){
				switch(evm.keyboard.keycode){
					case ALLEGRO_KEY_1:
					menu=0;
					choose=1;
					al_stop_sample_instance(menuMusic);
					break;
					case ALLEGRO_KEY_2:
					menu=0;
					choose=2;
					al_stop_sample_instance(menuMusic);
					break;
					case ALLEGRO_KEY_ESCAPE:
					menu=0;
					playing=0;
					end=0;
					start=0;
					break;
				}
			}
		}

		while(playing){
			drawScenario(display, choose);
			al_flip_display();
			ALLEGRO_EVENT ev;
			if (choose == 1){
				al_draw_bitmap(background1, 0, 0, 0);
				al_play_sample_instance(lvl1Audio);
			}
			else if (choose == 2){
				al_draw_bitmap(background2, 0, 0, 0);
				al_play_sample_instance(lvl2Audio);
			}
			al_wait_for_event(event_queue, &ev);
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					playing = 0;
				}
			}
			else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && !flag_animation) {
				getCell(ev.mouse.x, ev.mouse.y, &lin_src, &col_src);
			}
			else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !flag_animation) {
				getCell(ev.mouse.x, ev.mouse.y, &lin_dst, &col_dst);
				//Permite trocar apenas os doces vizinhos
				if(candyDistance(lin_src, col_src, lin_dst, col_dst) == 1
					&& M[lin_src][col_src].type && M[lin_dst][col_dst].type) {
					change(lin_src, col_src, lin_dst, col_dst);
					flag_animation = 1; 
					//Enquanto tem animação, não pode realizar movimento
				}
				if(candyDistance(lin_src, col_src, lin_dst, col_dst) == 2
					&& M[lin_src][col_src].type && M[lin_dst][col_dst].type) {
					change(lin_src, col_src, lin_dst, col_dst);
					flag_animation = 1; 
					//Enquanto tem animação, não pode realizar movimento
				}
				if(candyDistance(lin_src, col_src, lin_dst, col_dst) == 3
					&& M[lin_src][col_src].type && M[lin_dst][col_dst].type) {
					change(lin_src, col_src, lin_dst, col_dst);
					flag_animation = 1; 
					//Enquanto tem animação, não pode realizar movimento
				}
			}		
			else if(ev.type == ALLEGRO_EVENT_TIMER) {
				pontos = processaMatriz();
				while(pontos > 0) {
					do {
						score += pontos;
						//Chama o cenário e os doces
						drawScenario(display, choose);
						al_flip_display();
						//Pausa para ver a animação
						pauseAnimation(timer);						
						atualizaOffset();
						atualizaMatriz();
						pontos = processaMatriz();		
					}while(pontos > 0);
					//Continua descendo os doces
					initCandies();
					pontos = processaMatriz();
				}
				//Reinicializa a tela
				drawScenario(display, choose);
				al_flip_display();		
				if(plays == 0)
					playing = 0;
				flag_animation = 0;
			}
			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				playing = 0;
			}
		} 
		
		while(end){
			ALLEGRO_EVENT evm;
			al_wait_for_event(event_queue, &evm);

			al_flip_display();
			int record;
			al_draw_bitmap(gameover, 0, 0, 0);

			sprintf(my_score, "Score: %d", score);
			al_draw_text(size_f, al_map_rgb(255, 255, 255), 355, (HEIGHT/2)-35, 0, my_score);

			if(newRecord(score, &record)) {
				al_draw_text(size_f, al_map_rgb(255, 255, 255), 295, (75+HEIGHT/2)-10, 0, "NEW  RECORD!");
			}
			else {
				sprintf(my_score, "Record: %d", record);
				al_draw_text(size_f, al_map_rgb(255, 255, 255), 335, (75+HEIGHT/2)-10, 0, my_score);
			}

			if(evm.type == ALLEGRO_EVENT_KEY_DOWN){
				switch(evm.keyboard.keycode){
					case ALLEGRO_KEY_S:
					menu=1;
					end=0;
					break;
					case ALLEGRO_KEY_N:
					start=0;
					end=0;
					break;
					case ALLEGRO_KEY_ESCAPE:
					start=0;
					end=0;
					break;
				}
			}
		}
	}
		
	//Destrói tudo
	al_destroy_bitmap(bgmenu);
	al_destroy_bitmap(background1);
	al_destroy_bitmap(background2);
	al_destroy_sample(bgmusic);
	al_destroy_sample_instance(menuMusic);
	al_destroy_sample(lvl1);
	al_destroy_sample_instance(lvl1Audio);
	al_destroy_sample(lvl2);
	al_destroy_sample_instance(lvl2Audio);
	al_destroy_bitmap(gameover);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	return 0;
}