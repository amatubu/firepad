//
//  FireworksMain.c
//  Fireworks screen saver
//
//  Created by naoki iimura on Sun Jul 14 2002.
//  Copyright (c) 2002 - 2010 naoki iimura. All rights reserved.
//


#define	DEBUG_FW_TYPE	0

#if TARGET_MAC || __APPLE_CC__
	//#include <AppKit/AppKit.h>

	#include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>    

    #ifndef TRUE
        #define	TRUE	1
    #endif
#else
	//#include <GL/gl.h>
	#include <gl.h>
#endif

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "FireworksMain.h"


/* difinitions */
#define	f		420.0f // 視差
#define	zNear	3000.0f
#define	zFar	6000.0f
#define	z0		3500.0f


/* global variables */
FW_PrefsRec	prefs = {
	DEFAULT_FW_NUM, 
	DEFAULT_PARTICLE_LIFETIME, 
	DEFAULT_GRAVITY, 
	
    COLORMODE_RANDOM, 
	STARSIZE_CONSTANT,
    
    DEFAULT_FPS,
    
    MODE_2D,
    HIDE_3D_GUIDE
};

float	screen_width,screen_height;

/* static variables */
static FWRec	*fireworks = NULL;
	
static short	fw_count = 0; // number of fireworks;
	
static float	ff[MAX_PARTICLE_LIFETIME],ffc[MAX_PARTICLE_LIFETIME][3];

static const short	CHANGE_COLOR_PARCENTAGE[] = { 85, 15, 0 };

static short	num_frames = 0; // number of frames;


/* local functions */
static void launchFireworks(short l);
static void launchingFireworks(short l);
static void openingFireworks(short l);

static void	launchingFireworksPostDraw(short l);
static void	openingFireworksPreDraw(short l);
static void	openingFireworksPostDraw(short l);

static void createFireworksColor(FW_ColorRec *fw_color);
static void createFireworksGradationColor(FW_ColorRec *fw_color_list);
static void blendTwoColors(const FW_ColorRec *c1, const FW_ColorRec *c2, FW_ColorRec *outColor);
static void	darkenColor(FW_ColorRec *color);
static void	lightenColor(FW_ColorRec *color);
static void initFireworksColorDivision(short *division_of_colors,short first_star,short last_star);
static void initChangeStarColorNum(short *num_stars_to_change,short first_star,short last_star);

static void	show3dGuide(void);


#define	FireworksList	1


// fireworks main routine
void displayFireworks(void)
{
	short	l;
	
//    glNewList(FireworksList,GL_COMPILE);
    
    for (l = 0; l < prefs.fw_num; l++)
    {
        switch (fireworks[l].m)
        {
            case FW_WAIT_FOR_LAUNCH_PHASE: // wait for launch
                launchFireworks(l);
                break;
            
            case FW_LAUNCHING_PHASE: // launch
                launchingFireworks(l);
                launchingFireworksPostDraw(l);
                break;
            
            case FW_OPENING_PHASE: // opening
                openingFireworksPreDraw(l);
                openingFireworks(l);
                openingFireworksPostDraw(l);
                break;
        }
    }

#if 0
    if (prefs.flag_3d && prefs.show_3d_guide)
        show3dGuide();
#endif
    
//    glEndList();

#if 0
    if (prefs.flag_3d)
    {
        float	w,h;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        w = (float)(screen_width/2);
        h = (float)(screen_height/2);
        
        // left eye view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustumf(-w + f, w + f, -h, h, zNear, zFar);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-w + f, -h, -z0);
        
//        glCallList(FireworksList);
        
        // right eye view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustumf(-w - f, w - f, -h, h, zNear, zFar);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-w - f, -h, -z0);
        
//        glCallList(FireworksList);
    }
    else
#endif
    {
//        glClear(GL_COLOR_BUFFER_BIT);
        
//        glCallList(FireworksList);
    }
    
    glFlush();
    
//    glDeleteLists(FireworksList,1);
    
    return;

#if 0
    if (prefs.flag_3d)
    {
        // 3D
        short	mode[MAX_FW_NUM];
        float	w,h;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        w = (float)(screen_width/2);
        h = (float)(screen_height/2);
        
        // pre draw
        
        // left eye view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustumf(-w + f, w + f, -h, h, zNear, zFar);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-w + f,-h,-z0);
        
        // show guide
        if (prefs.show_3d_guide)
        	show3dGuide();
        for (l = 0; l < prefs.fw_num; l++)
        {
            mode[l] = fireworks[l].m;
            
            if (mode[l] == FW_OPENING_PHASE)
                openingFireworksPreDraw(l);
        }
        
        // right eye view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustumf(-w - f, w - f, -h, h, zNear, zFar);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-w - f,-h,-z0);
        
        for (l = 0; l < prefs.fw_num; l++)
        {
            if (mode[l] == FW_OPENING_PHASE)
                openingFireworksPreDraw(l);
        }
        
        // move
        for (l = 0; l < prefs.fw_num; l++)
        {
            switch (mode[l])
            {
                case FW_WAIT_FOR_LAUNCH_PHASE:
                    launchFireworks(l);
                    break;
                
                case FW_LAUNCHING_PHASE:
                    launchingFireworks(l);
                    break;
                
                case FW_OPENING_PHASE:
                    openingFireworks(l);
                    break;
            }
        }
        
        // post draw
        
        // right eye view (continue)
        for (l = 0; l < prefs.fw_num; l++)
        {
            switch (mode[l])
            {
                case FW_WAIT_FOR_LAUNCH_PHASE:
                    break;
                
                case FW_LAUNCHING_PHASE:
                    launchingFireworksPostDraw(l);
                    break;
                
                case FW_OPENING_PHASE:
                    openingFireworksPostDraw(l);
                    break;
            }
        }
        
        // show guide
        if (prefs.show_3d_guide)
        	show3dGuide();
        
        // left eye view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustumf(-w + f, w + f, -h, h, zNear, zFar);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(-w + f,-h,-z0);
        
        for (l = 0; l < prefs.fw_num; l++)
        {
            switch (mode[l])
            {
                case FW_WAIT_FOR_LAUNCH_PHASE:
                    break;
                
                case FW_LAUNCHING_PHASE:
                    launchingFireworksPostDraw(l);
                    break;
                
                case FW_OPENING_PHASE:
                    openingFireworksPostDraw(l);
                    break;
            }
        }
        
        // show guide
        if (prefs.show_3d_guide)
        	show3dGuide();
    }
    else
#endif
    {
        // 2D
        glClear(GL_COLOR_BUFFER_BIT);
//        glClearColor(prefs.background_color.red,prefs.background_color.green,
//                prefs.background_color.blue,1.0f);
        
        for (l = 0; l < prefs.fw_num; l++)
        {
            switch (fireworks[l].m)
            {
                case FW_WAIT_FOR_LAUNCH_PHASE: // wait for launch
                    launchFireworks(l);
                    break;
                
                case FW_LAUNCHING_PHASE: // launch
                    launchingFireworks(l);
                    launchingFireworksPostDraw(l);
                    break;
                
                case FW_OPENING_PHASE: // opening
                    openingFireworksPreDraw(l);
                    openingFireworks(l);
                    openingFireworksPostDraw(l);
                    break;
            }
        }
    }
    
    num_frames ++; // count up number of frames
    
	glFlush();
}

void launchFireworks(short l)
{
	FWRec	*firePtr = &fireworks[l];
    
    // count down
	firePtr->c--;
	
	// launch
	if (firePtr->c <= 0)
	{
		firePtr->m = FW_LAUNCHING_PHASE; // launch
		
		// setting of the firework
		firePtr->x = frand() * (screen_width - 100.0f)+50.0f;
		firePtr->y = 0.0f;
		firePtr->vx = frand() * 0.5f - 0.25f;
		firePtr->vy = (sqrt(screen_height * G1 * 8 + 1) - 1) / 2 - 5.0f + frand() * 5.0f;
		
		// color of firework
		createFireworksColor(&firePtr->fw_color);
		
		// type of firework
		#if DEBUG_FW_TYPE
			firePtr->fw_type = DEBUG_FW_TYPE;
		#else
			firePtr->fw_type = lrand() % FW_NUM_OF_TYPES;
		#endif
		
		fw_count ++;
	}
}

void launchingFireworks(short l)
{
	FWRec	*firePtr = &fireworks[l];
    
	firePtr->x += firePtr->vx;
	firePtr->y += firePtr->vy;
	
	// gravity
	firePtr->vy -= G1;
	firePtr->vx *= 0.975f;
	
	if (firePtr->vy < 0.5f)
	{
		int	i,j;
		float	angle,fw_size,s1,zAngle;
		
		firePtr->m = FW_OPENING_PHASE; // open
		
		// size of the firework
		fw_size = frand()*4.0f+2.5f;
		
		// reset the position of stars
		for (i = 0; i < MAX_PARTICLES; i++)
		{
			for (j = 0; j < prefs.ai_lifetime; j++)
			{
				firePtr->px[i][j] = firePtr->x;
				firePtr->py[i][j] = firePtr->y;
                firePtr->pz[i][j] = 0;
			}
		}
		
		// flash
		{
			float	x = firePtr->x,
					y = firePtr->y;
			
			// enable alpha-channel blend
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			for (j = 0; j < 10; j++)
			{
				float	r = fw_size * (j+1)/2.2f;
				
				glColor4f(1.0f,
						1.0f,
						1.0f,
						(10-j)/18.0f);
				// draw circle
//				glBegin(GL_POLYGON);
                GLfloat poly[20 * 2 + 2 * 2];
                poly[0] = x;
                poly[1] = y;
				for (i = 1; i < 20 + 2; i++)
				{
                    poly[i*2]   = x + r * cos(i*pi/10);
                    poly[i*2+1] = y + r * sin(i*pi/10);
//					glVertex2f(x + r * cos(i*pi/10), y + r * sin(i*pi/10));
				}
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_FLOAT, 0, poly);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 22);
//				glEnd();
			}
			glDisable(GL_BLEND);
		}
		
		firePtr->trace_length = prefs.ai_lifetime; // by default chrysanthemum (KIKU)
		firePtr->num_stars = PARTICLES;
		
		if (prefs.star_size_mode == STARSIZE_VARIABLE)
			firePtr->star_size = 2.0f + frand() * frand() * 1.0f;
		
		// effect of firework
		firePtr->fw_effect = 0;
		if (TRUE || lrand() % 8 == 0) // 1/8
		{
			firePtr->fw_effect |= FW_EFFECT_CHANGE_COLOR;
			if (lrand() % 3 == 0) // 1/3
				firePtr->fw_effect |= FW_EFFECT_CHANGE_COLOR_LAST;
		}
		if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR_LAST) ==0 & lrand() % 4 == 0) // 1/4
			firePtr->fw_effect |= FW_EFFECT_BLINK;
		
		// lifetime of firework
		firePtr->c = 40+(int)(fw_size * 3) + 
			prefs.ai_lifetime + EXTRA_LIFETIME;
		
		// timing of color change
		if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0)
		{
			if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR_LAST) != 0)
				firePtr->change_time = firePtr->c - EXTRA_LIFETIME * 2;
			else
				firePtr->change_time = 15+(lrand() % 15)+(int)(fw_size * 1.6);
		}
		
        // temprarily
//		firePtr->fw_type = FW_TYPE_DOUBLE_PISTIL;
		
		// the type of firework
		switch (firePtr->fw_type)
		{
			#pragma mark FW_TYPE_NORMAL
            case FW_TYPE_NORMAL:
				for (i = 0; i < PARTICLES; i++)
				{
					angle = frand() * pi*2;
                    zAngle = frand() * pi/2;
					s1 = fw_size * cos(zAngle) + 0.2f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy + G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle);
				}
				
				if (rand() & 1)
					firePtr->trace_length = 0; // peony (BOTAN)
				
				if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0) // change color
				{
					firePtr->num_colors = GRADATION_COLOR_NUM - 1;
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*GRADATION_COLOR_NUM);
					firePtr->star_colors[0] = firePtr->fw_color;
					createFireworksColor(&firePtr->star_colors[2]);
					createFireworksGradationColor(&firePtr->star_colors[0]);
					initFireworksColorDivision(&firePtr->division_of_colors[0],0,PARTICLES);
					initChangeStarColorNum(&firePtr->num_stars_to_change[0],0,PARTICLES);
					firePtr->change_start = 1;
				}
				else
					firePtr->num_colors = 0;
				break;
			
            #pragma mark FW_TYPE_HALF
			case FW_TYPE_HALF: /* half and half */
				{
					float	halfAngle = frand() * pi*2;
					float	halfAngle2 = -frand() * pi/2;
					
					// half
					for (i = 0; i < PARTICLES / 2; i++)
					{
						angle = frand()*pi + halfAngle;
                        zAngle = frand()*pi + halfAngle2;
						//s1 = frand()*fw_size+0.2;
						s1 = fw_size*cos(zAngle)+0.2f;
						
						firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
						firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy + G1;
                        firePtr->vpz[i] = fw_size * sin(zAngle);
					}
					
					// the other half
					for (i = PARTICLES / 2; i < PARTICLES; i++)
					{
						angle = frand() * pi + halfAngle + pi;
                        zAngle = frand() * pi + halfAngle2;
						s1 = fw_size*cos(zAngle)+0.2f;
						
						firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
						firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy + G1;
                        firePtr->vpz[i] = fw_size * sin(zAngle);
					}
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*2);
					firePtr->star_colors[0] = firePtr->fw_color;
					
					createFireworksColor(&firePtr->star_colors[1]);
				}
				
				if (rand() & 1)
					firePtr->trace_length = 0; // peony (BOTAN)
				
                firePtr->fw_effect &= ~FW_EFFECT_CHANGE_COLOR; // don't change color
                
				firePtr->num_colors = 1;
				firePtr->division_of_colors[0] = 0;
				firePtr->division_of_colors[1] = PARTICLES / 2;
				firePtr->division_of_colors[2] = PARTICLES;
				break;
			
            #pragma mark FW_TYPE_OVAL
			case FW_TYPE_OVAL: /* oval (sunflower?) */
                zAngle = frand() * pi;
                
				{
					float	fw_size2 = fw_size * cos(zAngle);
					float	s1;
					float	ovalTilt = frand() * pi;
					float	co1 = fw_size * cos(ovalTilt),
							co2 = fw_size2 * cos(ovalTilt),
							si1 = fw_size * sin(ovalTilt),
							si2 = fw_size2 * sin(ovalTilt);
					
					for (i = 0; i < PARTICLES; i++)
					{
						angle = frand() * pi*2;
						
						s1 = (frand() + 19) / 20;
						
						firePtr->vpx[i] = s1 * (co1 * cos(angle) - si2 * sin(angle))
											+ firePtr->vx;
						firePtr->vpy[i] = s1 * (si1 * cos(angle) + co2 * sin(angle))
											+ firePtr->vy+G1;
                        firePtr->vpz[i] = s1 * (fw_size * sin(angle) * sin(zAngle));
					}
				}
				
				firePtr->trace_length = 0; // peony (BOTAN) only
				
				if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0) // change color
				{
					firePtr->num_colors = GRADATION_COLOR_NUM - 1;
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*GRADATION_COLOR_NUM);
					firePtr->star_colors[0] = firePtr->fw_color;
					createFireworksColor(&firePtr->star_colors[2]);
					createFireworksGradationColor(&firePtr->star_colors[0]);
					initFireworksColorDivision(&firePtr->division_of_colors[0],0,PARTICLES);
					initChangeStarColorNum(&firePtr->num_stars_to_change[0],0,PARTICLES);
					firePtr->change_start = 1;
				}
				else
					firePtr->num_colors = 0;
				break;
			
            #pragma mark FW_TYPE_DOUBLE_OVAL
			case FW_TYPE_DOUBLE_OVAL: /* double oval */
                zAngle = frand() * pi;
                
				{
					// oval #1
					float	fw_size2 = fw_size * cos(zAngle);
					float	s1;
					float	ovalTilt = frand() * pi;
					float	co1 = fw_size * cos(ovalTilt),
							co2 = fw_size2 * cos(ovalTilt),
							si1 = fw_size * sin(ovalTilt),
							si2 = fw_size2 * sin(ovalTilt);
					
					for (i = 0; i < PARTICLES / 2; i++)
					{
						angle = frand() * pi*2;
						
						s1 = (frand() + 19) / 20;
						
						firePtr->vpx[i] = s1 * (co1 * cos(angle) - si2 * sin(angle))
											+ firePtr->vx;
						firePtr->vpy[i] = s1 * (si1 * cos(angle) + co2 * sin(angle))
											+ firePtr->vy+G1;
                        firePtr->vpz[i] = s1 * (fw_size * sin(angle) * sin(zAngle));
					}
					
					// oval #2
                    zAngle = frand() * pi;
					fw_size2 = fw_size * cos(zAngle);
					ovalTilt = ovalTilt + (pi / 2)*(frand()+9)/10;
					co1 = fw_size * cos(ovalTilt);
					co2 = fw_size2 * cos(ovalTilt);
					si1 = fw_size * sin(ovalTilt);
					si2 = fw_size2 * sin(ovalTilt);
					
					for (i = PARTICLES / 2; i < PARTICLES; i++)
					{
						angle = frand() * pi*2;
						
						s1 = (frand() + 19) / 20;
						
						firePtr->vpx[i] = s1 * (co1 * cos(angle) - si2 * sin(angle))
											+ firePtr->vx;
						firePtr->vpy[i] = s1 * (si1 * cos(angle) + co2 * sin(angle))
											+ firePtr->vy+G1;
                        firePtr->vpz[i] = s1 * (fw_size * sin(angle) * sin(zAngle));
					}
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*2);
					firePtr->star_colors[0] = firePtr->fw_color;
					
					createFireworksColor(&firePtr->star_colors[1]);
				}
				
				firePtr->trace_length = 0; // peony (BOTAN) only
				
                firePtr->fw_effect &= ~FW_EFFECT_CHANGE_COLOR; // don't change color
                
				firePtr->num_colors = 1;
				firePtr->division_of_colors[0] = 0;
				firePtr->division_of_colors[1] = PARTICLES / 2;
				firePtr->division_of_colors[2] = PARTICLES;
				break;
			
            #pragma mark FW_TYPE_OVAL_AND_NORMAL
			case FW_TYPE_OVAL_AND_NORMAL: /* normal and oval */
				// normal
				for (i = 0; i < PARTICLES * 2/3; i++)
				{
					angle = frand() * pi*2;
                    zAngle = frand() * pi/2;
					s1 = fw_size * cos(zAngle) + 0.2f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle);
				}
				
				// oval
				fw_size = (fw_size + 0.2f) * (frand()*0.2f+1.2f);
                zAngle = frand() * pi/2 + pi/4;
				
				{
					float	fw_size2 = fw_size * cos(zAngle);
					float	s1;
					float	ovalTilt = frand() * pi;
					float	co1 = fw_size * cos(ovalTilt),
							co2 = fw_size2 * cos(ovalTilt),
							si1 = fw_size * sin(ovalTilt),
							si2 = fw_size2 * sin(ovalTilt);
					
					for (i = PARTICLES * 2/3; i < PARTICLES; i++)
					{
						angle = frand() * pi*2;
						
						s1 = (frand() + 19) / 20;
						
						firePtr->vpx[i] = s1 * (co1 * cos(angle) - si2 * sin(angle))
											+ firePtr->vx;
						firePtr->vpy[i] = s1 * (si1 * cos(angle) + co2 * sin(angle))
											+ firePtr->vy+G1;
                        firePtr->vpz[i] = s1 * (fw_size * sin(angle) * sin(zAngle));
					}
				}
				
				firePtr->trace_length = 0; // currently peony (BOTAN) only
				
				if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0) // change color
				{
					firePtr->num_colors = GRADATION_COLOR_NUM;
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*(GRADATION_COLOR_NUM+1));
					
                    if ((lrand() & 1) == 0) // 1/2
                    {
                        // change normal part
                        firePtr->star_colors[0] = firePtr->fw_color;
                        createFireworksColor(&firePtr->star_colors[2]);
                        createFireworksGradationColor(&firePtr->star_colors[0]);
                        initFireworksColorDivision(&firePtr->division_of_colors[0],0,PARTICLES * 2/3);
                        initChangeStarColorNum(&firePtr->num_stars_to_change[0],0,PARTICLES * 2/3);
                        firePtr->change_start = 1;
                        
                        createFireworksColor(&firePtr->star_colors[3]);
                        firePtr->division_of_colors[4] = PARTICLES;
                    }
                    else
                    {
                        // change oval part
                        firePtr->star_colors[0] = firePtr->fw_color;
                        firePtr->division_of_colors[0] = 0;
                        
                        createFireworksColor(&firePtr->star_colors[1]);
                        createFireworksColor(&firePtr->star_colors[3]);
                        createFireworksGradationColor(&firePtr->star_colors[1]);
                        initFireworksColorDivision(&firePtr->division_of_colors[1],PARTICLES * 2/3,PARTICLES);
                        initChangeStarColorNum(&firePtr->num_stars_to_change[0],PARTICLES * 2/3,PARTICLES);
                        firePtr->change_start = 2;
                    }
				}
                else
                {
                    firePtr->num_colors = 1;
                    
                    firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*2);
                    firePtr->star_colors[0] = firePtr->fw_color;
                    
                    createFireworksColor(&firePtr->star_colors[1]);
                    firePtr->division_of_colors[0] = 0;
                    firePtr->division_of_colors[1] = PARTICLES * 2/3;
                    firePtr->division_of_colors[2] = PARTICLES;
                }
				break;
			
            #pragma mark FW_TYPE_WITH_PISTIL
			case FW_TYPE_WITH_PISTIL: /* with pistil (SHIN) */
				fw_size *= 1.2f;
				firePtr->num_stars = MAX_PARTICLES;
				
				for (i = 0; i < MAX_PARTICLES * 2/3; i++)
				{
					angle = frand() * pi*2;
					zAngle = frand() * pi/2;
					s1 = fw_size * cos(zAngle) + 0.8f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle) + 0.4f;
				}
				
				fw_size = fw_size * (frand()*0.3f + 0.3f);
				
				// pistil
				for (i = MAX_PARTICLES * 2/3; i < MAX_PARTICLES; i++)
				{
					angle = frand() * pi*2;
					zAngle = frand() * pi/2;
					s1 = fw_size*cos(zAngle) + 0.1f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle);
				}
				
				if ((lrand() & 3) == 0) // 1/4
					firePtr->trace_length = 0; // peony (BOTAN)
				
				if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0) // change color
				{
					firePtr->num_colors = GRADATION_COLOR_NUM;
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*(GRADATION_COLOR_NUM+1));
					
                    if ((lrand() & 1) == 0) // 1/2
                    {
                        // change normal part
                        firePtr->star_colors[0] = firePtr->fw_color;
                        createFireworksColor(&firePtr->star_colors[2]);
                        createFireworksGradationColor(&firePtr->star_colors[0]);
                        initFireworksColorDivision(&firePtr->division_of_colors[0],0,MAX_PARTICLES * 2/3);
                        initChangeStarColorNum(&firePtr->num_stars_to_change[0],0,MAX_PARTICLES * 2/3);
                        firePtr->change_start = 1;
                        
                        createFireworksColor(&firePtr->star_colors[3]);
                        firePtr->division_of_colors[4] = MAX_PARTICLES;
                    }
                    else
                    {
                        // change pistil part
                        firePtr->star_colors[0] = firePtr->fw_color;
                        firePtr->division_of_colors[0] = 0;
                        
                        createFireworksColor(&firePtr->star_colors[1]);
                        createFireworksColor(&firePtr->star_colors[3]);
                        createFireworksGradationColor(&firePtr->star_colors[1]);
                        initFireworksColorDivision(&firePtr->division_of_colors[1],MAX_PARTICLES * 2/3,MAX_PARTICLES);
                        initChangeStarColorNum(&firePtr->num_stars_to_change[0],MAX_PARTICLES * 2/3,MAX_PARTICLES);
                        firePtr->change_start = 2;
                    }
                }
                else
                {
                    firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*2);
                    firePtr->star_colors[0] = firePtr->fw_color;
                    
                    createFireworksColor(&firePtr->star_colors[1]);
                    
                    firePtr->num_colors = 1;
                    firePtr->division_of_colors[0] = 0;
                    firePtr->division_of_colors[1] = MAX_PARTICLES * 2/3;
                    firePtr->division_of_colors[2] = MAX_PARTICLES;
                }
				break;
			
            #pragma mark FW_TYPE_DOUBLE_PISTIL
			case FW_TYPE_DOUBLE_PISTIL: /* with double pistil (YAE-SHIN) */
				fw_size *= 1.5f;
				firePtr->num_stars = MAX_PARTICLES;
				
				for (i = 0; i < MAX_PARTICLES / 2; i++)
				{
					angle = frand() * pi*2;
					zAngle = frand() * pi/2;
					s1 = fw_size * cos(zAngle) + 0.8f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle) + 0.4f;
				}
				
				fw_size = fw_size * (frand() * 0.3f + 0.3f);
				
				// pistil #1
				for (i = MAX_PARTICLES / 2; i < MAX_PARTICLES * 5/6; i++)
				{
					angle = frand() * pi*2;
					zAngle = frand() * pi/2;
					s1 = fw_size * cos(zAngle)+0.3f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle) + 0.2f;
				}
				
				fw_size = fw_size * (frand() * 0.3f + 0.3f);
				
				// pistil #2
				for (i = MAX_PARTICLES * 5/7; i < MAX_PARTICLES; i++)
				{
					angle = frand() * pi*2;
					zAngle = frand() * pi/2;
					s1 = fw_size*cos(zAngle)+0.1f;
					
					firePtr->vpx[i] = s1 * cos(angle) + firePtr->vx;
					firePtr->vpy[i] = s1 * sin(angle) + firePtr->vy+G1;
                    firePtr->vpz[i] = fw_size * sin(zAngle);
				}
				
				if ((rand() & 3) == 0) // 1/4
					firePtr->trace_length = 0; // peony (BOTAN)
				
				if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0) // change color
				{
					firePtr->num_colors = GRADATION_COLOR_NUM + 1; // +1 = (+2 -1)
					
					firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*(GRADATION_COLOR_NUM+2));
					
                    switch (lrand() % 3)
                    {
                        case 0:
                            // change normal part
                            firePtr->star_colors[0] = firePtr->fw_color;
                            createFireworksColor(&firePtr->star_colors[2]);
                            createFireworksGradationColor(&firePtr->star_colors[0]);
                            initFireworksColorDivision(&firePtr->division_of_colors[0],0,MAX_PARTICLES * 1/2);
                            initChangeStarColorNum(&firePtr->num_stars_to_change[0],0,MAX_PARTICLES * 1/2);
                            firePtr->change_start = 1;
                            
                            createFireworksColor(&firePtr->star_colors[3]);
                            createFireworksColor(&firePtr->star_colors[4]);
                            firePtr->division_of_colors[4] = MAX_PARTICLES * 5/6;
                            firePtr->division_of_colors[5] = MAX_PARTICLES;
                            break;
                        
                        case 1:
                            // change pistil#1 part
                            firePtr->star_colors[0] = firePtr->fw_color;
                            firePtr->division_of_colors[0] = 0;
                            
                            createFireworksColor(&firePtr->star_colors[1]);
                            createFireworksColor(&firePtr->star_colors[3]);
                            createFireworksGradationColor(&firePtr->star_colors[1]);
                            initFireworksColorDivision(&firePtr->division_of_colors[1],MAX_PARTICLES * 1/2,MAX_PARTICLES * 5/6);
                            initChangeStarColorNum(&firePtr->num_stars_to_change[0],MAX_PARTICLES * 1/2,MAX_PARTICLES * 5/6);
                            firePtr->change_start = 2;
                            
                            createFireworksColor(&firePtr->star_colors[4]);
                            firePtr->division_of_colors[5] = MAX_PARTICLES;
                            break;
                        
                        case 2:
                            // change pistil#2 part
                            firePtr->star_colors[0] = firePtr->fw_color;
                            createFireworksColor(&firePtr->star_colors[1]);
                            firePtr->division_of_colors[0] = 0;
                            firePtr->division_of_colors[1] = MAX_PARTICLES * 1/2;
                            
                            createFireworksColor(&firePtr->star_colors[2]);
                            createFireworksColor(&firePtr->star_colors[4]);
                            createFireworksGradationColor(&firePtr->star_colors[2]);
                            initFireworksColorDivision(&firePtr->division_of_colors[2],MAX_PARTICLES * 5/6,MAX_PARTICLES);
                            initChangeStarColorNum(&firePtr->num_stars_to_change[0],MAX_PARTICLES * 5/6,MAX_PARTICLES);
                            firePtr->change_start = 3;
                            break;
                    }
                }
                else
                {
                    firePtr->star_colors = (FW_ColorRec *)malloc(sizeof(FW_ColorRec)*3);
                    firePtr->star_colors[0] = firePtr->fw_color;
                    
                    createFireworksColor(&firePtr->star_colors[1]);
                    createFireworksColor(&firePtr->star_colors[2]);
                    
                    firePtr->num_colors = 2;
                    firePtr->division_of_colors[0] = 0;
                    firePtr->division_of_colors[1] = MAX_PARTICLES / 2;
                    firePtr->division_of_colors[2] = MAX_PARTICLES * 5/6;
                    firePtr->division_of_colors[3] = MAX_PARTICLES;
                }
				break;
		}
		
		/* if type of firework is chrysanthemum, blink effect does not used */
		if (firePtr->trace_length > 0)
			firePtr->fw_effect &= ~FW_EFFECT_BLINK;
	}
}

void launchingFireworksPostDraw(short l)
{
    FWRec	*firePtr = &fireworks[l];
    
    if (firePtr->m != FW_LAUNCHING_PHASE) return;
    
    // 打ち上げの光を表示
    glPointSize(PARTICLE_SIZE);
//    glBegin(GL_POINTS);
//    glColor4f(1.0f,1.0f,1.0f,1.0f);
//    glVertex2f(firePtr->x,firePtr->y);
//    glEnd();
    GLfloat pt[2];
    pt[0] = firePtr->x;
    pt[1] = firePtr->y;
    glVertexPointer(2, GL_FLOAT, 0, pt);
    glDrawArrays(GL_POINTS, 0, 2);
}

void openingFireworksPreDraw(short l)
{
	int	i,j,k,m;
    FWRec	*firePtr = &fireworks[l];
    
    if (firePtr->c >= firePtr->trace_length)
		k = 0;
	else
		k = firePtr->trace_length - firePtr->c;
	
	// wipe the trace (O)
	if (prefs.star_size_mode == STARSIZE_VARIABLE)
		glPointSize(firePtr->star_size);
	
	// display the trace (O)
	for (j = k; j < firePtr->trace_length; j++)
	{
//		glBegin(GL_POINTS);
		
		if (firePtr->num_colors == 0)
		{
			glColor4f(ff[j] * firePtr->fw_color.red + ffc[j][0],
					ff[j] * firePtr->fw_color.green + ffc[j][1],
					ff[j] * firePtr->fw_color.blue + ffc[j][2],
                      1.0f);
            GLfloat pts[firePtr->num_stars * 3];
			for (i = 0; i < firePtr->num_stars; i++) {
//				glVertex2f(firePtr->px[i][j],firePtr->py[i][j]);
//                glVertex3f(firePtr->px[i][j],firePtr->py[i][j],firePtr->pz[i][j]);
                pts[i*3  ] = firePtr->px[i][j];
                pts[i*3+1] = firePtr->py[i][j];
                pts[i*3+2] = firePtr->pz[i][j];
            }
            glVertexPointer(3, GL_FLOAT, 0, pts);
            glDrawArrays(GL_POINTS, 0, firePtr->num_stars);
		}
		else
		{
            GLfloat pts[firePtr->num_stars*3];
            for (i = 0; i < firePtr->num_stars; i++) {
                //						glVertex2f(firePtr->px[i][j],firePtr->py[i][j]);
                //						glVertex3f(firePtr->px[i][j],firePtr->py[i][j],firePtr->pz[i][j]);
                pts[i*3  ] = firePtr->px[i][j];
                pts[i*3+1] = firePtr->py[i][j];
                pts[i*3+2] = firePtr->pz[i][j];
            }
            glVertexPointer(3, GL_FLOAT, 0, pts);
			for (m = 0; m <= firePtr->num_colors; m++)
			{
				glColor4f(ff[j] * firePtr->star_colors[m].red + ffc[j][0],
						ff[j] * firePtr->star_colors[m].green + ffc[j][1],
						ff[j] * firePtr->star_colors[m].blue + ffc[j][2],
                          1.0f);
                glDrawArrays(GL_POINTS, firePtr->division_of_colors[m],
                             firePtr->division_of_colors[m+1] -
                             firePtr->division_of_colors[m]);
			}
		}
//		glEnd();
	}
}

void openingFireworks(short l)
{
	int	i,j;
    FWRec	*firePtr = &fireworks[l];
	
	// move stars (HOSHI)
	for (i = 0; i < firePtr->num_stars; i++)
	{
		for (j = firePtr->trace_length - 1; j > 0; j--)
		{
			firePtr->px[i][j] = firePtr->px[i][j-1];
			firePtr->py[i][j] = firePtr->py[i][j-1];
            firePtr->pz[i][j] = firePtr->pz[i][j-1];
		}
		firePtr->px[i][0] += firePtr->vpx[i];
		firePtr->py[i][0] += firePtr->vpy[i];
        firePtr->pz[i][0] += firePtr->vpz[i];
		
		firePtr->vpy[i] -= prefs.gravity;
		firePtr->vpy[i] *= 0.975f;//0.99;
		firePtr->vpx[i] *= 0.975f;//0.99;
        firePtr->vpz[i] *= 0.975f;
	}
	
	// change color effect
	if ((firePtr->fw_effect & FW_EFFECT_CHANGE_COLOR) != 0)
	{
		firePtr->change_time --;
		if (firePtr->change_time < GRADATION_COLOR_NUM)
		{
			short	change_start,change_time;
			short	*division_of_colors;
			
			change_time = firePtr->change_time;
			
			if (change_time >= - GRADATION_COLOR_NUM + 2)
			{
				change_start = firePtr->change_start;
				division_of_colors = &firePtr->division_of_colors[change_start];
				
				for (i=1; i<=GRADATION_COLOR_NUM - 2; i++)
					division_of_colors[i] = division_of_colors[i-1];
				
				if (change_time >= 0)
				{
					division_of_colors[0] = 
						firePtr->num_stars_to_change[GRADATION_COLOR_NUM - 1 - change_time];
				}
			}
		}
	}
	
	firePtr->c--;
	if (firePtr->c <= 0) // finish
	{
		firePtr->m = FW_WAIT_FOR_LAUNCH_PHASE; // wait for launch
		firePtr->c = (int)(frand()*25+15+prefs.fw_num); // time to launch
		
		if (firePtr->star_colors)
		{
			free(firePtr->star_colors);
			firePtr->star_colors = NULL;
		}
	}
	else // if (firePtr->c > PARTICLE_LIFETIME)
	{
		/* blink */
		if (firePtr->c < BLINK_TIME && (firePtr->fw_effect & FW_EFFECT_BLINK) != 0)
		{
			for (i = 0; i < firePtr->num_stars; i++)
				if (firePtr->px[i][0] > screen_width * 4)
					firePtr->px[i][0] -= (screen_width * 8);
			
			for (i = 0; i < firePtr->num_stars / 2; i++)
				firePtr->px[rand() % firePtr->num_stars][0] +=
					screen_width * 8;
		}
		
		/* erase stars gradually */
		if (firePtr->c < EXTRA_LIFETIME)
		{
			for (i = 0; i < firePtr->num_stars / 4; i++)
				firePtr->px[rand() % firePtr->num_stars][0] = -screen_width * 2;
		}
	}
}

void openingFireworksPostDraw(short l)
{
    int	i,m;
    FWRec	*firePtr = &fireworks[l];
    
    // do nothing when opening phase ended
    if (firePtr->m != FW_OPENING_PHASE) return;

    // display the stars (HOSHI)
//    glBegin(GL_POINTS);
    
    if (firePtr->num_colors == 0) // single color
    {
        glColor4f(firePtr->fw_color.red,
                firePtr->fw_color.green,
                firePtr->fw_color.blue,
                  1.0f);
        GLfloat pts[firePtr->num_stars*3];
        for (i = 0; i < firePtr->num_stars; i++) {
//            glVertex2f(firePtr->px[i][0],firePtr->py[i][0]);
//            glVertex3f(firePtr->px[i][0],firePtr->py[i][0],firePtr->pz[i][0]);
            pts[i*3  ] = firePtr->px[i][0];
            pts[i*3+1] = firePtr->py[i][0];
            pts[i*3+2] = firePtr->pz[i][0];
        }
        glVertexPointer(3, GL_FLOAT, 0, pts);
        glDrawArrays(GL_POINTS, 0, firePtr->num_stars);
    }
    else // multi colors
    {
        GLfloat pts[firePtr->num_stars*3];
        for (i = 0; i < firePtr->num_stars; i++) {
            //                    glVertex2f(firePtr->px[i][0],firePtr->py[i][0]);
            //                    glVertex3f(firePtr->px[i][0],firePtr->py[i][0],firePtr->pz[i][0]);
            pts[i*3  ] = firePtr->px[i][0];
            pts[i*3+1] = firePtr->py[i][0];
            pts[i*3+2] = firePtr->pz[i][0];
        }
        glVertexPointer(3, GL_FLOAT, 0, pts);
        for (m = 0; m <= firePtr->num_colors; m++)
        {
            glColor4f(firePtr->star_colors[m].red,
                    firePtr->star_colors[m].green,
                    firePtr->star_colors[m].blue,
                      1.0f);
            glDrawArrays(GL_POINTS, firePtr->division_of_colors[m],
                         firePtr->division_of_colors[m+1] -
                         firePtr->division_of_colors[m]);
        }
    }
//    glEnd();
    
    glPointSize(PARTICLE_SIZE);
}

// reset the fireworks
void initFireworks(void)
{
	int	l,j;
	
	if (fireworks) free(fireworks);
	// allocate memory for fireworks
	fireworks = (FWRec *)malloc(sizeof(FWRec) * prefs.fw_num);
	
	for (l = 0; l < prefs.fw_num; l++)
	{
		fireworks[l].m = FW_WAIT_FOR_LAUNCH_PHASE; // wait for launch
		fireworks[l].c = (int)(frand()*35) + l * 10; // time to launch
		
		fireworks[l].star_colors = NULL;
	}
	
	for (j = 0; j < prefs.ai_lifetime; j++)
    {
		ff[j] = ((float)(prefs.ai_lifetime - j-1) / (float)prefs.ai_lifetime);
        ffc[j][0] = prefs.background_color.red * (1 - ff[j]);
        ffc[j][1] = prefs.background_color.green * (1 - ff[j]);
        ffc[j][2] = prefs.background_color.blue * (1 - ff[j]);
    }
}

// the end of fireworks
void finishFireworks(void)
{
	free(fireworks);
    fireworks = NULL;
}

const FW_ColorRec	palette[] = {
	{ 1.0f, 1.0f, 1.0f }, // white
	{ 1.0f, 0.2f, 0.2f }, // red
	{ 1.0f, 1.0f, 0.2f }, // yellow
	{ 0.2f, 0.2f, 1.0f }, // blue
	{ 0.2f, 1.0f, 0.2f }, // green
	{ 1.0f, 0.2f, 1.0f }, // violet
	{ 0.2f, 0.7f, 1.0f } // water color
};

// create color of fireworks
void createFireworksColor(FW_ColorRec *fw_color)
{
	switch (prefs.color_mode)
	{
		case COLORMODE_RANDOM:
			// 0.6 - 1.0
			fw_color->red = frand()*0.4f + 0.6f;
			fw_color->blue = frand()*0.4f + 0.6f;
			fw_color->green = frand()*0.4f + 0.6f;
			break;
		
		case COLORMODE_PALETTE:
			fw_color[0] = palette[lrand() % 7];
			break;
	}
}

// create gradation color of fireworks
void createFireworksGradationColor(FW_ColorRec *fw_color_list)
{
	if ((lrand() % 10) == 0)
        darkenColor(&fw_color_list[0]);
    else if ((lrand() % 10) == 0)
        lightenColor(&fw_color_list[2]);
    
	blendTwoColors(&fw_color_list[0], &fw_color_list[2], &fw_color_list[1]);
}

// blend two colors
void blendTwoColors(const FW_ColorRec *c1, const FW_ColorRec *c2, FW_ColorRec *outColor)
{
    outColor->red = (c1->red + c2->red)/2;
	outColor->green = (c1->green + c2->green)/2;
	outColor->blue = (c1->blue + c2->blue)/2;
}

// darken color
void darkenColor(FW_ColorRec *color)
{
    color->red = color->red / 11.0f + prefs.background_color.red * 10 / 11.0f;
    color->green = color->green / 11.0f + prefs.background_color.green * 10 / 11.0f;
    color->blue = color->blue / 11.0f + prefs.background_color.blue * 10 / 11.0f;
}

// lighten color
void lightenColor(FW_ColorRec *color)
{
    color->red *= 1.2f;
    color->green *= 1.2f;
    color->blue *= 1.2f;
    
    if (color->red > 1.0f) color->red = 1.0f;
    if (color->green > 1.0f) color->green = 1.0f;
    if (color->blue > 1.0f) color->blue = 1.0f;
}

// initialize gradation division of fireworks
void initFireworksColorDivision(short *division_of_colors,short first_star,short last_star)
{
	short	i;
	
	division_of_colors[0] = first_star;
	for (i=1; i<=GRADATION_COLOR_NUM; i++)
		division_of_colors[i] = last_star;
}

// initialize number of stars to change colors
void initChangeStarColorNum(short *num_stars_to_change,short first_star,short last_star)
{
	short	i;
    short	num_stars = last_star - first_star;
	
	for (i=0; i<GRADATION_COLOR_NUM; i++)
		num_stars_to_change[i] = first_star + (num_stars * CHANGE_COLOR_PARCENTAGE[i] + 50)/100;
}

// reshape OpenGL viewport and reset
void reshape(int width, int height)
{
 	if (prefs.flag_3d)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
 	
	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
#if 0
    if (prefs.flag_3d)
        glFrustumf(-(GLfloat)width/2, (GLfloat)width/2, 
            -(GLfloat)height/2, (GLfloat)height/2,
            zNear, zFar);
    else
#endif
        glOrthof(0, (GLfloat)width, 0, (GLfloat)height, -10000, 10000);
	
    glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	
    initBackground();
}

// initialize background
void initBackground(void)
{
	glPointSize(PARTICLE_SIZE);
    
    glClearColor(prefs.background_color.red,prefs.background_color.green,
            prefs.background_color.blue,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

// show 3d guide
void show3dGuide(void)
{
    if (prefs.show_3d_guide == SHOW_3D_GUIDE_BLINKING) {
        float	guide_luminance = (1 - cos(num_frames * pi / 40)) * 0.15f; // loop by 80 frames
        
        glColor4f(guide_luminance * (2 - prefs.background_color.red) + 0.1f,
                guide_luminance * (2 - prefs.background_color.green) + 0.1f,
                guide_luminance * (2 - prefs.background_color.blue) + 0.1f,
                  1.0f);
    } else {
        glColor4f(0.3f * (2 - prefs.background_color.red) + 0.1f,
            0.3f * (2 - prefs.background_color.green) + 0.1f,
            0.3f * (2 - prefs.background_color.blue) + 0.1f,
                  1.0f);
    }
#if 0
    glBegin(GL_POINTS);
    glVertex2f(screen_width / 2, 0.0f);
    glEnd();
#endif
}
