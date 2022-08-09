# jogodeTenis
## Jogo de Tênis feito usando a linguagem C e a biblioteca Allegro, para o trabalho da disciplina de PDS1 da UFMG. 
## ===========================================================================	


### 1. Introdução
##### O Trabalho Prático sugerido para a disciplina de Programação e Desenvolvimento de
Software I foi baseado na criação de um jogo utilizando a biblioteca Allegro
(www.allegro.cc), que foi apresentado aos estudantes no decorrer do semestre. A proposta
inicial do projeto era de que, por intermédio dessa ferramenta, os estudantes pudessem
criar seu próprio jogo, que seguisse uma série de funcionalidades e regras.
No jogo em questão, dois jogadores se enfrentam em uma partida de tênis com múltiplas
bolas, a princípio sem um fim definido. As partidas são contabilizadas em sets de 10 pontos,
definindo como ganhador final aquele que venceu o maior número de sets (caso os
jogadores não desejam jogar partidas de 10 em 10, o número de pontos marcados até o
momento da finalização do jogo definirá quem é o ganhador). As bolas saem de cada um
dos 4 cantos da quadra, começando pelo superior direito e seguindo em sentido horário,
com um novo lançamento a cada 5 segundos. Cada bola possui um raio, cor e velocidade
aleatórios. Para cada vez que um participante falha em defender a bola de passar por sua
parte da quadra, é dado um ponto para seu adversário, placar este que é mostrado ao vivo
na parte superior da tela. O limite de bolas que saem de cada canto da tela é de 20
### ===========================================================================	
### 2. Teclas e utilização
##### O jogador 1 (o de baixo) se movimenta utilizando as teclas A, W, S e D do teclado, e só pode rebater uma bola quando clicando na tecla de Espaço. Já o jogador 2 (o de cima), usa as 4 teclas de setas para se deslocar, e rebate as bolas com o botão de Enter. Para sair do jogo, é só clicar no botão “X” no canto superior direito da tela. Uma nova tela pretaaparecerá por 4 segundos, mostrando o placar da partida e quem foi o vencedor, e o programa, por fim, será automaticamente fechado.
### ===========================================================================	
### 3. Funções e Procedimentos
##### No começo do jogo, uma série de variáveis globais e definições são feitas para facilitar o
acesso a informações importantes ao longo do código, e também para agilizar futuras
modificações. A exemplo: definição da velocidade, o raio e o número mínimo das bolas na
partida. Já as entidades do jogo foram declaradas como structs para o melhor arranjo do
sistema. São elas as structs: Coordenada (X e Y), Jogador (P1 e P2), Bola, e Canhão (para
o lançamento das bolas). Para cada uma delas (menos a de Coordenada), existem três
funções: uma para inicializá-las, outra para atualizá-las e outra para desenhá-las.
● Bola CriaBolaRandom: função que designa um raio, uma cor, e uma velocidade
aleatórios para todas as bolas que serão criadas a partir dela, além de definir em
que canto da tela elas irão aparecer e, de acordo com essas coordenadas, delimitar
para qual lado elas devem ser arremessadas. Por exemplo: as bolas originadas no
canto superior esquerdo, deverão ser arremessadas para a direita e para baixo, para
que não saiam da tela.
● void AdicionaBola: procedimento para criar sempre uma bola no vetor de bolas de
cada canhão, e também contabilizar quantas bolas estão em campo, para que esse
número não ultrapasse um limite pré-estabelecido.
● void MovimentaBola: procedimento para definir a velocidade com que cada bola se
movimenta;
● void MantemBolaNaTela: procedimento para delimitar as colisões com a quadra. A
bola deve ricochetear e voltar para o campo quando bater nas paredes laterais, mas
deve sumir quando passa pelas partes superiores ou inferiores. Exemplo: se a bola
vinha da direita e bateu na parede da esquerda, seu movimento em X deve ser
invertido, para que ela volte para a direita.
● void bolaOnouOff: procedimento para verificar se a bola saiu ou não pelos limites
superiores e inferiores da quadra, permitindo, assim, a contabilização dos pontos.
Por conta do timer do Allegro, uma série de problemas impossibilitaram uma
implementação mais simples dessa função. A estratégia abordada foi que: a partir
de um limite imaginário instaurado poucos pixels antes do fim da quadra, todas as
bolas que passarem por ele, e suas respectivas velocidades apontarem que elas
estavam indo na direção de saída, e não de entrada da quadra, serão contabilizadas
como fora.
● void colisaoJogadorBola: procedimento para verificar a colisão entre jogador e
bola, e permitir o rebote. Sempre que a bola entra em contato com o jogador, sua
trajetória será devidamente invertida, e ela é voltada para para a direção do
oponente. Em relação ao eixo X, sua direção é aleatória, ou seja, usando da função
randSinal (que só retorna 1 ou -1) ela pode voltar tanto para a esquerda, quanto
para a direita.
● void recomecaJogo: procedimento para verificar quando um dos jogadores atingiu
10 pontos, zerando a pontuação de ambos novamente e assinalando o ganho de um
set e o começo de outro.
● void quemGanhou: caso os jogadores não tenham jogado por um set inteiro (os
pontos de ambos são menos de 10), quem ganhou é quem obteve mais pontos
durante esse período. Já quando 1 ou mais partidas foram jogadas, o ganhador é
aquele que obteve mais vitórias totais.● ALLEGRO_BITMAP *imagem e ALLEGRO_SAMPLE *music: designados,
respectivamente, para imagem de fundo de tela do jogo (que está acompanhada dos
arquivos, e foi criada por autoria própria, utilizando a plataforma canva) e música de
fundo (que pode ser encontrada no link: https://youtu.be/l7SwiFWOQqM).
● Funcionamento dos canhões: 4 canhões são criados, com suas respectivas
coordenadas nos cantos das telas. Para que eles funcionem na ordem especificada,
um contador é adicionado em um loop, de forma que o canhão de ID 0 é o primeiro a
começar. Esse contador é acrescentado em 1 a cada canhão que passa, e quando
chega no último é zerado novamente, reiniciando o ciclo. Nesse for, uma série de
funções são atribuídas a cada bola do vetor de bolas no canhão, mas elas só são
mesmo criadas posteriormente, usando o tempo % 5 para que isso aconteça de 5
em 5 segundos.
● Funções de Sprintf e al_draw_text foram usadas para escrever diretamente na tela
tanto o placar ao vivo, quanto as mensagens finais da partida, que são visualizadas
após o término do jogo.
● Eventos de tecla foram utilizados para possibilitar a movimentação dos jogadores, e
também para que as bolas possam apenas ser rebatidas com o pressionar do botão
de espaço ou de enter.
● Ps: por fim, algumas funções, como “colisaoParedao” (usada anteriormente para
tentar contar quantas vezes as bolas saíram pelas extremidades, a partir da criação
de um limite exterior à quadra), e “newRecord” (arquivo que lê e atualiza quando
novos recordes são alcançados), não funcionaram como o esperado, mas seus
procedimentos foram deixados no código para possíveis futuras utilizações.








