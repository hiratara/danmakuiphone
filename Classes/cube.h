#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

typedef struct DanmakuColor{
    GLfloat r;
    GLfloat g;
    GLfloat b;
} DanmakuColor;

typedef struct Cube{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat vx;
    GLfloat vy;
    GLfloat vz;
    GLfloat size;
    DanmakuColor color;
    int cnt;
} Cube;

typedef struct Player{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat v;
    GLfloat distX;
    GLfloat distY;
    GLfloat size;
    int isDeleted;
    int cnt;
} Player;

#define FIRES 1000
typedef struct Fire{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat vx;
    GLfloat vy;
    GLfloat vz;
} Fire;

typedef struct Bomb{
    int isDeleted;
    int cnt;
    Fire fires[FIRES];
} Bomb;

typedef struct Task{
    void *data;
    void (*move)(void *);
    void (*draw)(void *);
} Task;

void initCube(Cube* c);
void moveCube(void* ptr);
void drawCube(void* ptr);

void movePlayer(void* ptr);
void drawPlayer(void* ptr);

void initBomb(Bomb *bomb, GLfloat x, GLfloat y, GLfloat z);
void moveBomb(void* ptr);
void drawBomb(void* ptr);

extern Player player;
#define CUBESIZE 100
extern Cube cubes[];
extern Task tasks[];
