#include "cube.h"
#include <stdlib.h>
#include <math.h>

Player player = {0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 50.0f, 0, 0};
Bomb   bomb;
Cube cubes[CUBESIZE];
Task tasks[CUBESIZE];

#define randFloat()  ( (float)rand() / (float)RAND_MAX )

void toVector(GLfloat xBase, GLfloat yBase, GLfloat x, GLfloat y, 
              GLfloat *vx, GLfloat *vy){
    GLfloat dx  = xBase - x;
    GLfloat dy  = yBase - y;
    GLfloat len = sqrt(dx * dx + dy * dy);
    if(len != 0){
        *vx = dx / len;
        *vy = dy / len;
    }else{
        *vx = *vy = 0;
    }
}

void initCube(Cube* c){
    float r = randFloat();
    if(r < 0.7){
        c->x = randFloat() * 400.0f - 200.0f;
        c->y = 250.0f;
    }else if(r < 0.8){
        c->x = 250.0f;
        c->y = randFloat() * 400.0f - 200.0f;
    }else if(r < 0.9){
        c->x = -250.0f;
        c->y = randFloat() * 400.0f - 200.0f;
    }else{
        c->x = randFloat() * 400.0f - 200.0f;
        c->y = -250.0f;
    }

    c->z  = 0.0f;

    GLfloat v  = randFloat() * 1.0f + 1.0f;

    if(randFloat() < 0.5){
        GLfloat th = randFloat() * 3.14 * 2;
        c->vx = v * cos(th);
        c->vy = v * sin(th);
        c->color.r = 1.0f;
        c->color.g = 1.0f;
        c->color.b = 0.0f;
    }else{
/*         GLfloat dx  = player.x - c->x; */
/*         GLfloat dy  = player.y - c->y; */
/*         GLfloat len = sqrt(dx * dx + dy * dy); */
/*         c->vx = dx / len * v; */
/*         c->vy = dy / len * v; */
        toVector(player.x, player.y, c->x, c->y, &(c->vx), &(c->vy));
        c->vx *= v;
        c->vy *= v;
        c->color.r = 0.0f;
        c->color.g = 1.0f;
        c->color.b = 0.0f;
    }

/*     if(randFloat() < 0.5){ */
/*         c->vy = randFloat() * 1.0f + 1.0f; */
/*     }else{ */
/*         c->vy = randFloat() * 1.0f - 2.0f; */
/*     } */
    c->vz = 0.0f;
    c->size = (randFloat() * 3.0f) * (randFloat() * 3.0f) + 4.0f;
    c->cnt = 0;
/*     printf("%f\n", randFloat()); */
}

void moveCube(void* ptr){
    Cube *c = ptr;
    if(c->vx == 0 && c->vy == 0 && c->vz == 0 
       || c->x < -250 || c->x > 250
       || c->y < -250 || c->y > 250
       || c->z < -250 || c->z > 250
       ){
        initCube(c);
    }

    c->cnt++;
    c->x += c->vx;
    c->y += c->vy;
    c->z += c->vz;
    //c->z = sin(3.14f / 60.0f * c->cnt) * 100;

    // 当たり判定
    GLfloat mx0 = c->x - c->size / 2.0f;
    GLfloat mx1 = c->x + c->size / 2.0f;
    GLfloat my0 = c->y - c->size / 2.0f;
    GLfloat my1 = c->y + c->size / 2.0f;

    GLfloat x0 = player.x - player.size / 2.0f;
    GLfloat x1 = player.x + player.size / 2.0f;
    GLfloat y0 = player.y - player.size / 2.0f;
    GLfloat y1 = player.y + player.size / 2.0f;

    if(! player.isDeleted &&
       mx0 < x1 && x0 < mx1 && my0 < y1 && y0 < my1){
        //printf("HIT!!!!!\n");
        if(player.size > c->size){
            player.size -= c->size;  //小さくなっちゃった
            initCube(c);             //弾は消す
        }else{
            // プレイヤー消滅
            player.isDeleted = 1;

            // プレイヤーのタスクを爆発タスクに書き換え
            initBomb(&bomb, player.x, player.y, player.z);
            tasks[0].data = &bomb;
            tasks[0].move = &moveBomb;
            tasks[0].draw = &drawBomb;
        }
    }
}

void drawCube(void* ptr){
    Cube *c = ptr;
    static const GLfloat squareVertices1[] = {
        0,0,0, 0,0,1, 0,1,1, 0,1,0, 0,0,0,
    };
    static const GLfloat squareVertices2[] = {
        1,0,0, 1,0,1, 1,1,1, 1,1,0, 1,0,0,
    };
    static const GLfloat squareVertices3[] = {
        0,0,0, 1,0,0,
        0,0,1, 1,0,1,
        0,1,0, 1,1,0,
        0,1,1, 1,1,1,
    };

    glColor4f(c->color.r, c->color.g, c->color.b, 1.0f);

    glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);

    glTranslatef(c->x, c->y, c->z);
    glScalef(c->size, c->size, c->size);
    glRotatef(1.1f * (float)c->cnt, 1.0f, 2.0f, 3.0f);
    glTranslatef(-0.5, -0.5, -0.5);
/*     printf("%d\n", c->cnt); */

    glVertexPointer(3, GL_FLOAT, 0, squareVertices1);
    glDrawArrays(GL_LINE_STRIP, 0, 5);

    glVertexPointer(3, GL_FLOAT, 0, squareVertices2);
    glDrawArrays(GL_LINE_STRIP, 0, 5);

    glVertexPointer(3, GL_FLOAT, 0, squareVertices3);
    glDrawArrays(GL_LINES, 0, 8);

    glPopMatrix();

    /* DEBUG */
    GLfloat debugpoints[] = {c->x, c->y, c->z};
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, debugpoints);
    glDrawArrays(GL_POINTS, 0, 1);
}

void movePlayer(void* ptr){
    Player *p = ptr;

    p->cnt++;

    GLfloat vx, vy;
    toVector(p->distX, p->distY, p->x, p->y, &vx, &vy);
    p->x += vx * p->v;
    p->y += vy * p->v;
    // p->x = p->v * cos((float)p->cnt * 3.14f / 1001.0f) * 100;
    // p->y = p->v * sin((float)p->cnt * 3.14f / 1001.0f) * 100;
    // p->z = sin(3.14f / 11.0f * p->cnt) * 100;
    p->z = 0.0f;
}

void drawPlayer(void* ptr){
    Player *p = ptr;
    static const GLfloat squareVertices1[] = {
        0,0,0, 0,0,1, 0,1,1, 0,1,0, 0,0,0,
    };
    static const GLfloat squareVertices2[] = {
        1,0,0, 1,0,1, 1,1,1, 1,1,0, 1,0,0,
    };
    static const GLfloat squareVertices3[] = {
        0,0,0, 1,0,0,
        0,0,1, 1,0,1,
        0,1,0, 1,1,0,
        0,1,1, 1,1,1,
    };

    //printf("%f %f %f %d\n", p->x, p->y, p->z, p->cnt);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);

    glTranslatef(p->x, p->y, p->z);
    glScalef(p->size, p->size, p->size);
    glRotatef(1.1f * (float)p->cnt, 1.0f, 2.0f, 3.0f);
    glTranslatef(-0.5, -0.5, -0.5);
/*     printf("%d\n", p->cnt); */

    glVertexPointer(3, GL_FLOAT, 0, squareVertices1);
    glDrawArrays(GL_LINE_STRIP, 0, 5);

    glVertexPointer(3, GL_FLOAT, 0, squareVertices2);
    glDrawArrays(GL_LINE_STRIP, 0, 5);

    glVertexPointer(3, GL_FLOAT, 0, squareVertices3);
    glDrawArrays(GL_LINES, 0, 8);

    glPopMatrix();

    /* DEBUG */
    GLfloat debugpoints[] = {p->x, p->y, p->z};
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertexPointer(3, GL_FLOAT, 0, debugpoints);
    glDrawArrays(GL_POINTS, 0, 1);
}


void initBomb(Bomb *bomb, GLfloat x, GLfloat y, GLfloat z){
    bomb->cnt = 0;
    for(int i = 0; i < FIRES; i++){
        bomb->fires[i].x = x;
        bomb->fires[i].y = y;
        bomb->fires[i].z = z;
        bomb->fires[i].vx = randFloat() * 10 - 5;
        bomb->fires[i].vy = randFloat() * 10 - 5;
        bomb->fires[i].vz = 0.0f;
        // bomb->fires[i].vz = randFloat() * 10;
    }
}

void moveBomb(void* ptr){
    Bomb *b = ptr;
    b->cnt++;

    for(int i = 0; i < FIRES; i++){
        b->fires[i].x += b->fires[i].vx;
        b->fires[i].y += b->fires[i].vy;
        b->fires[i].z += b->fires[i].vz;
        // b->fires[i].vz -= 0.0f;
    }
}

void drawBomb(void* ptr){
    Bomb *b = ptr;
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    if(b->isDeleted || b->cnt > 100){
        b->isDeleted = 1;
        return;
    }
    float rate = 1.0f - (float)b->cnt / 100.0f;

    glEnableClientState(GL_VERTEX_ARRAY);

    for(int i = 0; i < FIRES; i++){
        GLfloat point[] = {
            b->fires[i].x, b->fires[i].y, b->fires[i].z
        };
        glColor4f(
                  randFloat() * rate, 
                  randFloat() * rate, 
                  randFloat() * rate, 
                  1.0f // 
                  );
        glVertexPointer(3, GL_FLOAT, 0, point);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}
