//
//  EAGLView.m
//  DanmakuIphone
//
//  Created by たらら on 09/02/14.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#include "cube.h"

#define USE_DEPTH_BUFFER 0

#define CAMERA_FAR 250.0f


// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

void drawFrame(){
   static const GLfloat squareVertices[] = {
        -100.0f,-100.0f,0.0f, 
         100.0f,-100.0f,0.0f, 
         100.0f, 100.0f,0.0f, 
        -100.0f, 100.0f,0.0f, 
        -100.0f,-100.0f,0.0f, 
   };
   glColor4f(0.0f, 0.0f, 0.5f, 1.0f);
   glVertexPointer(3, GL_FLOAT, 0, squareVertices);
   glEnableClientState(GL_VERTEX_ARRAY);
   glDrawArrays(GL_LINE_STRIP, 0, 5);
}

@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;


// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
        animationInterval = 1.0 / 60.0;
    }

    /* こんなとこでやっちゃだめ */
    tasks[0].data = &player;
    tasks[0].move = &movePlayer;
    tasks[0].draw = &drawPlayer;
    for(int i = 1; i < CUBESIZE; i++){
        (cubes + i)->vx = (cubes + i)->vy = (cubes + i)->vz = 0;
        tasks[i].data = &cubes[i];
        tasks[i].move = &moveCube;
        tasks[i].draw = &drawCube;
    }

    return self;
}

// Cube c = {
//     0.5f, 0.5f, 0.0f,
//     0.001f, 0.002f, 0.0f,
//     0.05f, 0
// };

- (void)drawView {
    // set EGL Context
    [EAGLContext setCurrentContext:context];

    // prepare EGL frame buffer
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);

    // set view port
    glViewport(0, 0, backingWidth, backingHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // setprojection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(-100.0f, 100.0f, 
               -100.0f * (float) backingHeight / backingWidth,
                100.0f * (float) backingHeight / backingWidth,
               CAMERA_FAR, CAMERA_FAR + 200.0f);
//     glOrthof(0.0f, 1.0f, 0.0f, (float) backingHeight / backingWidth, 
//              -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, - (CAMERA_FAR + 100.0f)); // カメラ位置調整

    drawFrame();
    for(int i = 0; i < CUBESIZE; i++){
        (*tasks[i].move)(tasks[i].data);
        //moveCube(cubes + i);
    }
    for(int i = 0; i < CUBESIZE; i++){
        (*tasks[i].draw)(tasks[i].data);
        //drawCube(cubes + i);
    }

    // render buffer
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (void)startAnimation {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
    self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
    [animationTimer invalidate];
    animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
    
    animationInterval = interval;
    if (animationTimer) {
        [self stopAnimation];
        [self startAnimation];
    }
}


- (void)dealloc {
    
    [self stopAnimation];
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
    [super dealloc];
}


-(CGPoint)translateLocate: (CGPoint) p{
    CGPoint ret;

    GLfloat screenWidth = 200.0f;
    GLfloat screenHight = 200.0f * backingHeight / backingWidth;
    GLfloat zZeroRate   = 1.0f + (100.0f - player.z) / CAMERA_FAR;

    ret.x = (p.x / backingWidth - 0.5f) * screenWidth * zZeroRate;
    ret.y = (1.0f - p.y / backingHeight - 0.5f) * screenHight * zZeroRate;

    return ret;
}

-(void)touches: (NSSet*)touches withEvent:(UIEvent*)event{
    UITouch *touch = [touches anyObject];
    CGPoint p = [self translateLocate: [touch locationInView: self] ];
    
    player.distX = p.x;
    player.distY = p.y;
}

- (void)touchesBegan: (NSSet*)touches withEvent:(UIEvent*)event{
    [self touches: touches withEvent: event];
}

- (void)touchesMoved: (NSSet*)touches withEvent:(UIEvent*)event{
    [self touches: touches withEvent: event];
}

@end
