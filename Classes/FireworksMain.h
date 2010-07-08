//
//  FireworksMain.h
//  Fireworks screen saver
//
//  Created by naoki iimura on Sun Jul 14 2002.
//  Copyright (c) 2002 -2010 naoki iimura. All rights reserved.
//


#define USE_OPENGL 1

/* fireworks constants */
#define	FW_WAIT_FOR_LAUNCH_PHASE	0
#define	FW_LAUNCHING_PHASE			1
#define	FW_OPENING_PHASE			2
#define	FW_EXTRA_PHASE				3

#define	FW_TYPE_NORMAL				0
#define	FW_TYPE_HALF				1
#define	FW_TYPE_OVAL				2
#define	FW_TYPE_DOUBLE_OVAL			3
#define	FW_TYPE_OVAL_AND_NORMAL		4
#define	FW_TYPE_WITH_PISTIL			5
#define	FW_TYPE_DOUBLE_PISTIL		6

#define	FW_NUM_OF_TYPES				7

#define	FW_EFFECT_NONE				0x0000
#define	FW_EFFECT_CHANGE_COLOR		0x0001 /* HENKA */
#define FW_EFFECT_CHANGE_COLOR_LAST	0x0002 /* KOHRO */
#define FW_EFFECT_BLINK				0x0004

#define	FW_ALL_EFFECTS_MASK			0x0007

#define	MAX_FW_NUM					30
#define MIN_FW_NUM					3
#define	DEFAULT_FW_NUM				12

#define	MAX_PARTICLE_LIFETIME		30
#define	MIN_PARTICLE_LIFETIME		1
#define	DEFAULT_PARTICLE_LIFETIME	5

#define	MAX_GRAVITY				0.2f
#define	MIN_GRAVITY				0.01f
#define	DEFAULT_GRAVITY			0.1f

#define	COLORMODE_RANDOM		0
#define	COLORMODE_PALETTE		1

#define	STARSIZE_CONSTANT		0
#define	STARSIZE_VARIABLE		1

#define	MAX_FPS					20.0f
#define	DEFAULT_FPS				20.0f
#define	MIN_FPS					10.0f

#define	MODE_2D					0
#define	MODE_3D					1

#define	HIDE_3D_GUIDE			0
#define	SHOW_3D_GUIDE_BLINKING	1
#define	SHOW_3D_GUIDE_STATIC	2

#define	DEFAULT_2D_3D_MODE		MODE_2D
#define	DEFAULT_3D_GUIDE		HIDE_3D_GUIDE

#define	PARTICLE_SIZE			2.0
#define EXTRA_LIFETIME			20
#define BLINK_TIME				40

#define GRADATION_COLOR_NUM		3

/* fireworks preferences */
#if USE_OPENGL
	#define	MAX_PARTICLES		426
	#define	PARTICLES			210
#else
	#define	MAX_PARTICLES		93
	#define PARTICLES 			59 //93
#endif

/* gravity */
#define	G1				0.2

#if !TARGET_MAC
	#define	pi	3.14159265358979f
#endif

/* float rand */
#if TARGET_MAC
	#define frand()	((float)random() / (float) LONG_MAX)
	#define	lrand()	random()
#else
	#define frand() ((float)rand() / (float) RAND_MAX)
	#define	lrand()	rand()
#endif

/* fireworks colors */
typedef struct {
	float	red;
	float	green;
	float	blue;
} FW_ColorRec;

typedef struct {
	short	fw_num;
	short	ai_lifetime;
	float	gravity;
	
	short	color_mode;
	short	star_size_mode;
    
    float	fps; // frames per second
    
    short	flag_3d; // 1 = 3D mode
    short	show_3d_guide; // show guide on 3D mode
    
    FW_ColorRec	background_color;
} FW_PrefsRec;

/* fireworks record */
typedef struct {
	float	x;
	float	y;
    float	z;
	float	vx;
	float	vy;
    float	vz;
	
	short	m;
	short	c;
	
	short	fw_type; /* type of fireworks */
	short	fw_effect;
	short	trace_length;
	short	num_stars;
	
	float	star_size;
	
	float	px[MAX_PARTICLES][MAX_PARTICLE_LIFETIME];
	float	py[MAX_PARTICLES][MAX_PARTICLE_LIFETIME];
    float	pz[MAX_PARTICLES][MAX_PARTICLE_LIFETIME];
	float	vpx[MAX_PARTICLES];
	float	vpy[MAX_PARTICLES];
    float	vpz[MAX_PARTICLES];
	
	FW_ColorRec	fw_color;
	short	num_colors;
	short	division_of_colors[6];
	FW_ColorRec	*star_colors;
	
	short	change_time;
	short	change_start;
	short	num_stars_to_change[GRADATION_COLOR_NUM];
} FWRec;


/* public functions */
void initFireworks(void);
void displayFireworks(void);
void finishFireworks(void);

void reshape(int width, int height);
void initBackground(void);