// =============================================================================
// VIZA656/CSCE647 at Texas A&M University
// RAYTRACER
// CREATED BY: AMBER PLAYLE
// =============================================================================


#include <stdio.h>
#include <iostream>
#include <GLUT/glut.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string.h>

#include <pthread.h>

#include <Eigen/Dense>

#include "Parse.h"


using namespace Eigen;
using namespace std;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
float width, height;
unsigned char *pixmap;
Scene* s;

float randX;
float randY;

#define num_thread 8
#define threads_sq (num_thread * num_thread)
#define samples 4
#define numFrames 120




Ray primRay[num_thread * num_thread];
color pixels[num_thread * num_thread];


void Animation(float f);
void Animation2(float f);
void Animation3(float f);

// =============================================================================
// TYPE_DEF CLASSES
// =============================================================================

class arg_struct {
public:
	int thread;
    int tx;
    int ty;
    Image image;
    arg_struct(){};
	arg_struct(int thread, Image img) : thread(thread), image(img)
	{};
	~arg_struct(){};
	
};

// =============================================================================
// RENDER FUNCTIONS
// =============================================================================

void* render(void * arguments)
{
	
    arg_struct *args = (arg_struct*)arguments;
	
	int current_t = args->thread;
    int current_tx = args->tx;
    int current_ty = args->ty;
	Image img = args->image;
	
    //////////////////////////////////////////
	//		INITIALIZE THE PRIMARY RAY		//
	//////////////////////////////////////////
    
    
    #ifndef MT_OPENMP
	#pragma omp	parallel
    {
	#pragma omp for schedule(dynamic) nowait
	#endif
		for (int y = (height/num_thread) * current_ty; y < (height/num_thread) * (current_ty + 1); y++) {
			for (int x = (width/num_thread) * current_tx; x < (width/num_thread) * (current_tx + 1); x++) {
				
				pixels[current_t].set(0.0,0.0,0.0);
                
                for (int numX = 0; numX < samples; numX++) {
                    for (int numY = 0; numY < samples; numY ++) {
                        
                        float X = x + ((float)numX/samples) + (randX/samples);
                        float Y = y + ((float)numY/samples) + (randY/samples);
                        float i = X/width;
                        float j = Y/height;
                        
                        
                        primRay[current_t] = s->camera->InitRay(i, j);
                        
                        pixels[current_t] += s->raytrace(primRay[current_t]);
                        
                    }
                    
                }
				color p = pixels[current_t]/powf(samples, 2);;
				img.setPixel(p, x, y);

				int i = (y * width + x) * 3;
				
				pixmap[i] = (p.r * 255);
				pixmap[i + 1] =  (p.g * 255);
				pixmap[i + 2] = (p.b * 255);
				
				
			}
		}
	#ifndef MT_OPENMP
	}
	#endif
	
	cout << "T " << current_t << " is finished" << endl;
	
	pthread_exit(NULL);
}



// =============================================================================
// MULTITHREADING FUNCTIONS
// =============================================================================


void multiThreading()
{
	cout << "Rendering ...\n";
	time_t start, end;
	
    //////////////////////////////////////
	//		INITIALIZE THE IMAGE		//
	//////////////////////////////////////
	
	
	stringstream image;
	image << "../images/img_" << s->timer.getTime() << ".png";
	Image img(width,height);
	
	time(&start);
	
	//////////////////////////////////
	//		CREATING THREADS		//
	//////////////////////////////////
	
	int rc;
	pthread_t threads[threads_sq];
	
	cout << "Starting " << threads_sq << " threads..." << endl;
	
    int current_t = 0;
	for (int y = 0; y < num_thread; y++) {
        for (int x = 0; x < num_thread; x++) {
            
            //creating the arguments for the render function
            arg_struct* args = new arg_struct();
            args->image = img;
            args->ty = y;
            args->tx = x;
            args->thread = current_t;
            
            rc = pthread_create(&threads[current_t], NULL, render, (void*)args);
            if (rc){
                cout << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
            current_t++;
            
        }	
	}

	for (int current_t = 0; current_t < threads_sq; current_t++) {
		rc = pthread_join(threads[current_t], NULL);
		if (rc) {
			cout << "Error: unable to join threads: " << rc << endl;
			exit(-1);
		}
	}

    
    //////////////////////////////////
	//		WRITE THE PNG IMAGE		//
	//////////////////////////////////
    
    
	img.WritePNG(image.str().c_str());
	
    
	time(&end);
	float diff = difftime(end, start);
	cout << "Ellapsed time is " << diff << " seconds" << endl;
}



// =============================================================================
// OpenGL Display and Mouse Processing Functions.
//
// You can read up on OpenGL and modify these functions, as well as the commands
// in main(), to perform more sophisticated display or GUI behavior. This code
// will service the bare minimum display needs for most assignments.
// =============================================================================
static void windowResize(int w, int h)
{   
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(w/2),0,(h/2),0,1); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity() ;
}
static void windowDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0,0);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixmap);
	glFlush();
}
static void processMouse(int button, int state, int x, int y)
{
	if(state == GLUT_UP)
		exit(0);               // Exit on mouse click.
}
static void init(void)
{
	glClearColor(1,1,1,1); // Set background color.
}



// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char *argv[])
{
    
    randX = 1.0 * rand()/ RAND_MAX;
    randY =  1.0 * rand()/ RAND_MAX;
    
	
	char scene[200] = "/Users/Amber/Documents/College_Classes/VIZA_656/Scenes/obj_test.sdf";
	//draw scene
	s = getScene(scene);

	width = s->camera->width;
	height = s->camera->height;
	
    
	int pixels = width * height * 3;
	pixmap = new unsigned char[pixels]; 
	
    /*
    for (int f = 0; f < 60; f++) {
    //    float f = 60;
        s->timer.setTime(f);
        cout << "rendering frame: " << f << endl;
        Animation3(f);
        multiThreading();
    
    }
    
    
    char nextscene[200] = "/Users/Amber/Documents/College_Classes/VIZA_656/Scenes/animation.sdf";
	//draw scene
	s = getScene(nextscene);
    
	width = s->camera->width;
    height = s->camera->height;
    
    for (int f = 35; f < 88; f++) {
        //float f = 50;
        s->timer.setTime(f + 100);
        cout << "rendering animation frame: " << f << endl;
        Animation2(f);
        multiThreading();
        
    }
    
    char final[200] = "/Users/Amber/Documents/College_Classes/VIZA_656/Scenes/final.sdf";
	//draw scene
	s = getScene(final);
    
	width = s->camera->width;
    height = s->camera->height;

    float f = 120;
    s->timer.setTime(f + 100);
    cout << "rendering frame final: " << f << endl;
    Animation(f);
    multiThreading();
    
    f = 0;
    s->timer.setTime(221);
    cout << "rendering frame final: " << f << endl;
    Animation(f);
    multiThreading();
    */
    
	
	
	
	// OpenGL Commands:
	// Once "glutMainLoop" is executed, the program loops indefinitely to all
	// glut functions.  
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100); // Where the window will display on-screen.
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Raytracer");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();
	
	return 0; //This line never gets reached. We use it because "main" is type int.
}


void Animation(float f)
{
    
    float tx = 0.01 * f * f - 2.025 * f + 25 - 25 * f / float(numFrames);
    float tz = (88.9 + 30.f) / float(numFrames) * f - 30;
    
 
    s->camera->position[0] = tx;
    s->camera->position[2] = tz;
    
    
    Vector3f lookAt;
    lookAt[0] = -0.00555 * f * f + 0.667 * f + 25 - 25 * f / float(numFrames);
    lookAt[1] = (3 - 10) / float(numFrames) * f + 10;
    lookAt[2] = (120 - 90) / float(numFrames) * f + 90;
    
    Vector3f dir = lookAt - s->camera->position;
    
    s->camera->viewV = dir.normalized();    
    
}

void Animation2(float f)
{
    
    Vector3f pos;
    pos <<  -126.f / numFrames * f + 63 , 5.0, 35.0;
    //cout << pos << endl;
    moveObject("sphere1", pos, 45, s);
    
    
    
}

void Animation3(float f)
{
    s->camera->scale = f / 30.0 -2.0;
}

