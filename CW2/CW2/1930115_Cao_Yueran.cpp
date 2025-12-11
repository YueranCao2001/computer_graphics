// A well-thought 3-dimensional scene
# define FREEGLUT_STATIC

// There are ASCII characters ranging from 0 to 127, a total of 128 characters
# define MAX_CHAR 128

// Define the calculator PI to finish the calculation related to circles or ellipses
# define PI 3.1415926

// Include part of the program
# include <GL/freeglut.h>
# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# include <math.h>

// Define global variables used in texture mapping
GLuint texture_road;
GLuint texture_water;
GLuint texture_grass;
GLuint texture_floor;
GLuint texture_red;
GLuint texture_window;
GLuint texture_wall;
GLuint texture_roof_cb;
GLuint texture_soil;


// Define the width and height of result window
GLfloat width = 1100.0f;
GLfloat height = 1000.0f;


// Define the rotation factors
GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

// Define the scale factors -> used to make zoom in or zoom out
GLfloat scale_factor = 1.0f;

// Define the translation factors
GLfloat translate_factor = 1.0f;

// Define scale factors for change size of trees
GLfloat scale_tree = 1.0f;

// Define factors to make boat move -> boat_y used to make animation
GLfloat boat_x = 0.0f;
GLfloat boat_y = 0.0f;
GLfloat boat_z = 0.0f;

// Define the status of boat in animation
int boat_status = 1;

// Declare refresh interval in ms -> larger, slower
int time_interval = 600;
int time_interval_ufo = 50;

// Declare incremental step for clouds
GLfloat step = 1;

// Define the factors to make UFO move
GLfloat ufo_x = 0.0f;
GLfloat ufo_y = 0.0f;
GLfloat ufo_z = 0.0f;


// Define the specular color of material -> default value
GLfloat specular_material[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Define the emissive color of material -> default value
GLfloat emissive_material[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

// Define the shininess of material -> range [0, 128]
// The smaller the value, the rougher the material, and the light emitted by the point light source irradiates it, which can also produce larger bright spots
const GLfloat shininess_material = 15.0f;

// Define the diffuse color of material
GLfloat diffuse_material[4] = { 0.2f, 0.32f, 0.8f, 1.0f };


// Initialize the light of this scene
void myinit(void)
{
	GLfloat light_direction[] = { 700.0f, 400.0f, 100.0f, 1.0f };
	GLfloat light_intensity[] = { 255.0f / 255.0f, 210.0f / 255.0f, 166.0f / 255.0f, 1.0f };
	GLfloat ambient_intensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	// Set the ambient light
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);

	// Set the light
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity);

	// Configure the color -> glColor
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


// Set the properties of material -> input: diffuse_material
void set_material(GLfloat diffuse_material_info[4])
{
	// Set the same value to make the material more realistic
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse_material_info);

	// Set the specular color of material
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material);

	// Set the emissive color of material
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive_material);

	// Set the shininess of material
	glMaterialf(GL_FRONT_AND_BACK, shininess_material, 0);
}


// Define a function to work in texture mapping
GLuint texture_mapping(const char* file_name)
{
	// The basic information of file
	GLint width;
	GLint height;
	GLint pixel_length;
	GLubyte* pixel_data = 0;
	FILE* pFile = NULL;

	// Define to exit if the file can not be read
	errno_t error;

	// Define to complete the texture ID assignment
	GLuint initial_textrue_ID = 0;
	GLuint final_texture_ID = 0;

	// If the file can not be opened, exit the program
	if (error = fopen_s(&pFile, file_name, "rb") != 0)
		exit(0);

	/* Read and store the basic information of open file's image */

	// reposition stream position indicator
	// SEEK_SET: Beginning of file
	// skip 16 bits from beginning for bmp files
	fseek(pFile, 0x0012, SEEK_SET);

	// get the width of image
	fread(&width, sizeof(width), 1, pFile);

	// get the height of image
	fread(&height, sizeof(height), 1, pFile);

	// read the data of image as pixel
	fseek(pFile, 54, SEEK_SET);

	// Calculate the number of bytes occupied by each row of pixels, and calculate the total number of pixel bytes based on this data
	// pixel data consists of three colors red, green and blue (Windows implement BGR)
	GLint pixel_line = width * 3;

	// pixel data width should be an integral multiple of 4, which is required by the .bmp file
	while (pixel_line % 4 != 0)
		++pixel_line;

	// pixel data length = width * height
	pixel_length = pixel_line * height;

	// Allocate memory based on the total number of pixel bytes
	pixel_data = (GLubyte*)malloc(pixel_length);

	if (pixel_data == 0)
	{
		fclose(pFile);
		return 0;
	}

	// Read pixel data
	if (fread(pixel_data, pixel_length, 1, pFile) <= 0)
	{
		free(pixel_data);
		fclose(pFile);
		return 0;
	}

	// Assign a new texture ID
	// glGenTextures: return n texture indexes according to texture parameters
	glGenTextures(1, &final_texture_ID);

	if (final_texture_ID == 0)
	{
		free(pixel_data);
		fclose(pFile);
		return 0;
	}

	// Bind the final texture ID, load the texture and set the texture parameters
    // Before binding, firstly obtain the initial binding texture ID so that it can be restored at the end
	GLint initial_ID = initial_textrue_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &initial_ID);
	glBindTexture(GL_TEXTURE_2D, final_texture_ID);

	// glTexParameteri(): texture filter function, GL_LINEAR -> linear filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// GL_TEXTURE_WRAP_: Texture packaging function to prevent the texture from going out of range (0 to 1)
	// S: recycle textures in the X direction
	// T: recycle textures in the Y direction
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// glTexEnvf: texture combiner -> set the pixel level
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// glTexImage2D(): the function is to generate a 2D texture according to the specified parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixel_data);

	// Restore the previous texture binding
	glBindTexture(GL_TEXTURE_2D, initial_ID);

	// Release memory
	free(pixel_data);

	return final_texture_ID;
}


// Force the openGL program to redraw the current window
void when_in_mainloop()
{
	glutPostRedisplay();
}


// Define the animation of boat
void timer_boat(int value)
{
	// boat move up
	if (boat_status == 1) 
	{
		boat_y += 6.0f;
		boat_status++;
	}
	// boat move down
	else if (boat_status == 2) {
		boat_y -= 6.0f;
		boat_status--;
	}

	when_in_mainloop();
	glutTimerFunc(time_interval, timer_boat, 1);
}


// Define the animation of UFO
void timer_ufo(int value)
{
	ufo_x += step;
	if (ufo_x >= 900.0f)
		ufo_x = -450.0f;
	else if (ufo_x <= -450.0f)
		ufo_x = 900.0f;

	ufo_z += step;
	if (ufo_z >= 900.0f)
		ufo_z = -450.0f;
	else if (ufo_z <= -450.0f)
		ufo_z = 900.0f;

	when_in_mainloop();
	glutTimerFunc(time_interval_ufo, timer_ufo, 1);
}


// Define all keyboard interactions in the program
void keyboard_input(unsigned char key, int x, int y)
{
	// "q" || "Q": quit
	if (key == 'q' || key == 'Q')
		exit(0);

	// "b" || "B": bigger -> tree
	if (key == 'b' || key == 'B')
		scale_tree += 0.1f;

	// "l" || "L": littler -> tree
	if (key == 'l' || key == 'L')
		scale_tree -= 0.1f;

	// "c" || "C": call back -> retrieve the size of tree
	if (key == 'c' || key == 'C')
		scale_tree = 1.0f;

	// "w" || "W": make boat move up
	if (key == 'w' || key == 'W')
		boat_z -= 1.0f;

	// "s" || "S": make boat move down
	if (key == 's' || key == 'S')
		boat_z += 1.0f;

	// "a" || "A": make boat move left
	if (key == 'a' || key == 'A')
		boat_x -= 1.0f;

	// "d" || "D": make boat move right
	if (key == 'd' || key == 'D')
		boat_x += 1.0f;

	// "h" || "H": go home, retrieve the position of boat
	if (key == 'h' || key == 'H')
	{
		boat_x = 0.0f;
		boat_z = 0.0f;
	}

	// "t" || "T": terminal, ufo stop moving
	if (key == 't' || key == 'T')
		step = 0;

	// "r" || "R": reset the speed of UFO
	if (key == 'r' || key == 'R')
		step = 1;
	
	// Refresh the window
	when_in_mainloop();
}


// some keys cannot be called by the glutKeyboardFunc(), we need to call it by using glutSpecialFunc()
// Define all special keys interactions in the program
void special_key_input(int key, int x, int y)
{
	// For rotation
	// "↑": Flip up
	if (key == GLUT_KEY_UP)
		rotate_x += 1.0f;

	// "↓": Flip down
	if (key == GLUT_KEY_DOWN)
		rotate_x -= 1.0f;

	// "←": Turn left
	if (key == GLUT_KEY_LEFT)
		rotate_y -= 1.0f;

	// "→": Turn right
	if (key == GLUT_KEY_RIGHT)
		rotate_y += 1.0f;


	// For zoom -> use statement for to zoom in or zoom out the values of x, y, z in the same time
	// "PgUp": zoom in
	if (key == GLUT_KEY_PAGE_UP)
		scale_factor += 0.01f;

	// "PgDn": zoom out
	if (key == GLUT_KEY_PAGE_DOWN)
		scale_factor -= 0.01f;

	//refresh window
	when_in_mainloop();
}

// Define all mouse interactions in the program
void mouse_input(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && step >= -49)
		step -= 1;
	else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON && step <= 49)
		step += 1;
}


// Method to draw string consists of either English or Chinese
void drawString(const char* str)
{
	int len;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// Count the number of characters
	// If it is a double byte character (such as Chinese character), two bytes are counted as one character
	// Otherwise, a byte counts as a character (English character)
	len = 0;
	for (int i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	// Convert mixed characters to wide characters
	int w_len = len + 1;
	wstring = (wchar_t*)malloc(w_len * sizeof(wchar_t));
	if (wstring == NULL)
		exit(-1);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// Output characters one by one
	for (int i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// Reclaim all temporary resources
	free(wstring);
	glDeleteLists(list, 1);
}


// Method to select font for drawing string
void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}


// A cuboid has eight points and six faces
// Define a construction for this cuboid, when we enter the innermost point, the other 7 points will be calculated -> reduce calculation
GLfloat cuboid[8][3];


// The construction of cuboid -> define eight points of a cuboid
void construct_cuboid(GLfloat x, GLfloat y, GLfloat z, GLfloat X, GLfloat Y, GLfloat Z)
{
	// The first point -> point p0
	cuboid[0][0] = x;
	cuboid[0][1] = y;
	cuboid[0][2] = z;

	// The second point -> point p1
	cuboid[1][0] = x;
	cuboid[1][1] = y;
	cuboid[1][2] = z + Z;

	// The third point -> point p2
	cuboid[2][0] = x + X;
	cuboid[2][1] = y;
	cuboid[2][2] = z + Z;

	// The fourth point -> point p3
	cuboid[3][0] = x + X;
	cuboid[3][1] = y;
	cuboid[3][2] = z;

	// Use statement for to finish the definition of last four points -> p4, p5, p6, p7
	for (int i = 0; i <= 3; i++) 
	{
		for (int j = 0; j <= 2; j++)
		{
			// when j equals to 1, we need to calculate the value y of each point
			if (j == 1)
				cuboid[i + 4][j] = cuboid[i][j] + Y;
			else
				cuboid[i + 4][j] = cuboid[i][j];
		}
	}
}


// build the cuboid -> draw corresponding six faces
void build_cuboid()
{
	// Front -> face0
	glBegin(GL_POLYGON);
	// We need to consider the rotation of the 3D model and the problem of light source illumination,
	// so the setting of the normal vector is essential -> glNormal3f()
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(cuboid[5][0], cuboid[5][1], cuboid[5][2]);
	glVertex3f(cuboid[6][0], cuboid[6][1], cuboid[6][2]);
	glVertex3f(cuboid[2][0], cuboid[2][1], cuboid[2][2]);
	glVertex3f(cuboid[1][0], cuboid[1][1], cuboid[1][2]);
	glEnd();

	// Back -> face1
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(cuboid[0][0], cuboid[0][1], cuboid[0][2]);
	glVertex3f(cuboid[3][0], cuboid[3][1], cuboid[3][2]);
	glVertex3f(cuboid[7][0], cuboid[7][1], cuboid[7][2]);
	glVertex3f(cuboid[4][0], cuboid[4][1], cuboid[4][2]);
	glEnd();

	// Left -> face2
	glBegin(GL_POLYGON);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(cuboid[1][0], cuboid[1][1], cuboid[1][2]);
	glVertex3f(cuboid[0][0], cuboid[0][1], cuboid[0][2]);
	glVertex3f(cuboid[4][0], cuboid[4][1], cuboid[4][2]);
	glVertex3f(cuboid[5][0], cuboid[5][1], cuboid[5][2]);
	glEnd();

	// Right -> face3
	glBegin(GL_POLYGON);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(cuboid[7][0], cuboid[7][1], cuboid[7][2]);
	glVertex3f(cuboid[6][0], cuboid[6][1], cuboid[6][2]);
	glVertex3f(cuboid[2][0], cuboid[2][1], cuboid[2][2]);
	glVertex3f(cuboid[3][0], cuboid[3][1], cuboid[3][2]);
	glEnd();

	// Top -> face4
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(cuboid[4][0], cuboid[4][1], cuboid[4][2]);
	glVertex3f(cuboid[7][0], cuboid[7][1], cuboid[7][2]);
	glVertex3f(cuboid[6][0], cuboid[6][1], cuboid[6][2]);
	glVertex3f(cuboid[5][0], cuboid[5][1], cuboid[5][2]);
	glEnd();

	// Bottom -> face5
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(cuboid[0][0], cuboid[0][1], cuboid[0][2]);
	glVertex3f(cuboid[1][0], cuboid[1][1], cuboid[1][2]);
	glVertex3f(cuboid[2][0], cuboid[2][1], cuboid[2][2]);
	glVertex3f(cuboid[3][0], cuboid[3][1], cuboid[3][2]);
	glEnd();
}


// Define irregular cuboid like trapezoid, also eight points, six faces
GLfloat trapezoid[8][3];


// The construction of irregular cuboid
void construct_trapezoid(GLfloat x, GLfloat y, GLfloat z, GLfloat X, GLfloat Y, GLfloat Z)
{
	// The first point -> p0
	trapezoid[0][0] = x;
	trapezoid[0][1] = y;
	trapezoid[0][2] = z;

	// The second point -> p1
	trapezoid[1][0] = x;
	trapezoid[1][1] = y;
	trapezoid[1][2] = z + Z;

	// The fifth point -> p4
	trapezoid[4][0] = x;
	trapezoid[4][1] = y + Y;
	trapezoid[4][2] = z;

	// The sixth point -> p5
	trapezoid[5][0] = x;
	trapezoid[5][1] = y + Y;
	trapezoid[5][2] = z + Z / 2;

	// Use statement for to get points -> p3, p2, p6 and p7
	for (int i = 0; i < 3; i++)
	{
		// For x axis, re-calculate the value of x
		if (i == 0) 
		{
			trapezoid[3][i] = trapezoid[0][i] + X;
			trapezoid[2][i] = trapezoid[1][i] + X;
			trapezoid[6][i] = trapezoid[4][i] + X;
			trapezoid[7][i] = trapezoid[5][i] + X;
		}
		else
		{
			trapezoid[3][i] = trapezoid[0][i];
			trapezoid[2][i] = trapezoid[1][i];
			trapezoid[6][i] = trapezoid[4][i];
			trapezoid[7][i] = trapezoid[5][i];
		}
	}
}


// Build the irregular cuboid -> draw corresponding six faces
void build_trapezoid()
{
	// Front -> face 0
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(trapezoid[5][0], trapezoid[5][1], trapezoid[5][2]);
	glVertex3f(trapezoid[6][0], trapezoid[6][1], trapezoid[6][2]);
	glVertex3f(trapezoid[2][0], trapezoid[2][1], trapezoid[2][2]);
	glVertex3f(trapezoid[1][0], trapezoid[1][1], trapezoid[1][2]);
	glEnd();

	// Back -> face 1
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(trapezoid[0][0], trapezoid[0][1], trapezoid[0][2]);
	glVertex3f(trapezoid[3][0], trapezoid[3][1], trapezoid[3][2]);
	glVertex3f(trapezoid[7][0], trapezoid[7][1], trapezoid[7][2]);
	glVertex3f(trapezoid[4][0], trapezoid[4][1], trapezoid[4][2]);
	glEnd();

	// Left -> face 2
	glBegin(GL_POLYGON);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(trapezoid[1][0], trapezoid[1][1], trapezoid[1][2]);
	glVertex3f(trapezoid[0][0], trapezoid[0][1], trapezoid[0][2]);
	glVertex3f(trapezoid[4][0], trapezoid[4][1], trapezoid[4][2]);
	glVertex3f(trapezoid[5][0], trapezoid[5][1], trapezoid[5][2]);
	glEnd();

	// Right -> face 3
	glBegin(GL_POLYGON);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(trapezoid[7][0], trapezoid[7][1], trapezoid[7][2]);
	glVertex3f(trapezoid[6][0], trapezoid[6][1], trapezoid[6][2]);
	glVertex3f(trapezoid[3][0], trapezoid[3][1], trapezoid[3][2]);
	glVertex3f(trapezoid[2][0], trapezoid[2][1], trapezoid[2][2]);
	glEnd();

	// Top -> face 4
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(trapezoid[4][0], trapezoid[4][1], trapezoid[4][2]);
	glVertex3f(trapezoid[7][0], trapezoid[7][1], trapezoid[7][2]);
	glVertex3f(trapezoid[6][0], trapezoid[6][1], trapezoid[6][2]);
	glVertex3f(trapezoid[5][0], trapezoid[5][1], trapezoid[5][2]);
	glEnd();

	// Bottom -> face 5
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(trapezoid[0][0], trapezoid[0][1], trapezoid[0][2]);
	glVertex3f(trapezoid[1][0], trapezoid[1][1], trapezoid[1][2]);
	glVertex3f(trapezoid[2][0], trapezoid[2][1], trapezoid[2][2]);
	glVertex3f(trapezoid[3][0], trapezoid[3][1], trapezoid[3][2]);
	glEnd();
}


// Function to draw tree_1
void draw_tree_1()
{
	// The body of tree
	glColor3f(210.0f / 255.0f, 105.0f / 255.0f, 30.0f / 255.0f);
	glPushMatrix();
	construct_cuboid(270.0f, 25.0f, 300.0f, 15.0f, 70.0f + (scale_tree - 1) * 50.0f, 15.0f);
	build_cuboid();
	glPopMatrix();

	// The leaves of tree -> drawed with five sphere -> same method in 2D
	glColor3f(0.0f, 100.0f / 255.0f, 0.0f);
	glPushMatrix();
	glTranslatef(277.5f, 90.0f + (scale_tree - 1) * 50.0f, 307.5f);

	// draw sphere -> glutSoildSphere(radius, slices, stacks)
	glutSolidSphere(25.0 * scale_tree, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(265.0f, 80.0f + (scale_tree - 1) * 50.0f, 307.5f);
	glutSolidSphere(25.0 * scale_tree, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(290.0f, 80.0f + (scale_tree - 1) * 50.0f, 307.5f);
	glutSolidSphere(25.0 * scale_tree, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(277.5f, 80.0f + (scale_tree - 1) * 50.0f, 320.0f);
	glutSolidSphere(25.0 * scale_tree, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(277.5f, 80.0f + (scale_tree - 1) * 50.0f, 295.0f);
	glutSolidSphere(25.0 * scale_tree, 20, 20);
	glPopMatrix();
}


// Function to draw tree_2
void draw_tree_2()
{
	// The body of tree
	glColor3f(210.0f / 255.0f, 105.0f / 255.0f, 30.0f / 255.0f);
	construct_cuboid(370.0f, 25.0f, 300.0f, 15.0f, 70.0f + (scale_tree - 1) * 50.0f, 15.0f);
	build_cuboid();

	// The leaves of tree -> draw with cone
	glColor3f(0.0f, 100.0f / 255.0f, 0.0f);
	glPushMatrix();
	glTranslatef(377.5f, 70.0f + (scale_tree - 1) * 50.0f, 307.5f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	// draw cone -> glutSoildCone(base, height, slices, stacks)
	glutSolidCone(25.0 * scale_tree, 60.0 * scale_tree, 20, 20);
	glPopMatrix();
}


// Function to draw tree_3
void draw_tree_3()
{
	// The body of tree
	glColor3f(210.0f / 255.0f, 105.0f / 255.0f, 30.0f / 255.0f);
	construct_cuboid(370.0f, 25.0f, 300.0f, 15.0f, 70.0f + (scale_tree - 1) * 50.0f, 15.0f);
	build_cuboid();

	// The leaves of tree -> draw with cone
	glColor3f(0.0f, 100.0f / 255.0f, 0.0f);
	glPushMatrix();
	glTranslatef(377.5f, 90.0f + (scale_tree - 1) * 50.0f, 307.5f);

	// draw cube -> glutSolidCube(size)
	glutSolidCube(50.0 * scale_tree);
	glPopMatrix();
}


// Function to manage trees
void manage_tree()
{
	// Right top corner
	glPushMatrix();
	glTranslatef(180.0f, -20.0f, -640.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(280.0f, -20.0f, -640.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(380.0f, -20.0f, -640.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(480.0f, -20.0f, -640.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(180.0f, -20.0f, -730.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(280.0f, -20.0f, -730.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(380.0f, -20.0f, -730.0f);
	draw_tree_3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(480.0f, -20.0f, -730.0f);
	draw_tree_3();
	glPopMatrix();

	
	// Left bottom corner
	glPushMatrix();
	glTranslatef(-300.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-380.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-460.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-540.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-620.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-700.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-780.0f, -20.0f, 450.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-300.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-380.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-460.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-540.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-620.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-700.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-780.0f, -20.0f, 560.0f);
	draw_tree_2();
	glPopMatrix();


	// Trees in the bottom
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 20.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 120.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 220.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 20.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 120.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 220.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -220.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -320.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -420.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, -220.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, -320.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, -420.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 455.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 545.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 455.0f);
	draw_tree_1();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 545.0f);
	draw_tree_1();
	glPopMatrix();
}


// Function to draw sun
void draw_sun()
{
	glColor3f(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	glTranslatef(810.0f, 830.0f, 350.0f);
	glutSolidSphere(60.0, 20, 20);
	glPopMatrix();
}

// build the ground of the 3D scene
void draw_ground()
{
	// Draw sun
	draw_sun();

	// Soil layer -> brown
	glColor3f(102.0f / 255.0f, 51.0f / 255.0f, 0.0f / 255.0f);
	construct_cuboid(-450.0f, -5.0f, -450.0f, 1350.0f, 5.0f, 1350.0f);
	build_cuboid();

	// Land surface -> grey (texture mapping to draw road between Central building and Science Building)
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(-450.0f, 0.0f, -450.0f, 1350.0f, 10.0f, 1350.0f);
	build_cuboid();

	// Enable the texture mapping
	glEnable(GL_TEXTURE_2D);

	// Load the corresponding texture
	glBindTexture(GL_TEXTURE_2D, texture_road);

	// delimit the vertices of a primitive or a group of like primitives
	// GL_QUADS: Treats each group of four vertices as an independent quadrilateral
	glBegin(GL_QUADS);

	// specify the ordinates of checkerboard facing the viewer squarely
	// set the current texture coordinates
	glTexCoord2f(0.0f, 0.0f);

	// specify a vertex, X Y Z
	glVertex3f(120.0f, 11.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(120.0f, 11.0f, -200.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(550.0f, 11.0f, -200.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(550.0f, 11.0f, 600.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);


	// Texture mapping for soil
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_soil);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, -5.1f, -450.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, -5.1f, 900.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, -5.1f, 900.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, -5.1f, -450.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Texture mapping for ground
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_roof_cb);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 10.01f, -450.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 10.01f, 900.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 10.01f, 900.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 10.01f, -450.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Texture mapping for forests
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	// Right top corner
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(525.0f, 10.1f, -450.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(525.0f, 10.1f, -300.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 10.1f, -300.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 10.1f, -450.0f);

	glEnd();

	// Left bottom corner
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 10.1f, 700.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 10.1f, 900.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(150.0f, 10.1f, 900.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(150.0f, 10.1f, 700.0f);

	glEnd();
	
	glDisable(GL_TEXTURE_2D);


	/* The area between the two roads->have trees */
	// Top part
	// Foundation
	glColor3f(224.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f);
	construct_cuboid(220.0f, 10.0f, -300.0f, 225.0f, 12.0f, -150.0f);
	build_cuboid();

	// On the step -> gress (texture mapping)
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	construct_cuboid(225.0f, 12.0f, -310.0f, 215.0f, 14.0f, -130.0f);
	build_cuboid();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(225.0f, 27.0f, -440.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(225.0f, 27.0f, -310.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(440.0f, 27.0f, -310.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(440.0f, 27.0f, -440.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);


	// Middle part
	// Foundation
	glColor3f(224.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f);
	construct_cuboid(220.0f, 10.0f, -200.0f, 225.0f, 12.0f, 800.0f);
	build_cuboid();

	// On the steps
	glColor3f(175.0f / 255.0f, 127.0f / 255.0f, 113.0f / 255.0f);
	construct_cuboid(225.0f, 12.0f, -190.0f, 215.0f, 14.0f, 780.0f);
	build_cuboid();

	// Texture mapping for steps
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_red);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(225.0f, 26.1f, -190.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(225.0f, 26.1f, 590.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(440.0f, 26.1f, 590.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(440.0f, 26.1f, -190.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// On the step -> gress (texture mapping), top part
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	construct_cuboid(240.0f, 14.0f, -170.0f, 185.0f, 15.0f, 300.0f);
	build_cuboid();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(240.0f, 30.0f, 130.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(240.0f, 30.0f, -170.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(425.0f, 30.0f, -170.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(425.0f, 30.0f, 130.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// On the step -> gress (texture mapping), bottom part
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	construct_cuboid(240.0f, 14.0f, 270.0f, 185.0f, 15.0f, 300.0f);
	build_cuboid();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(240.0f, 30.0f, 570.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(240.0f, 30.0f, 270.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(425.0f, 30.0f, 270.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(425.0f, 30.0f, 570.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);


	// Bottom part
	// Foundation
	glColor3f(224.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f);
	construct_cuboid(220.0f, 10.0f, 700.0f, 225.0f, 12.0f, 200.0f);
	build_cuboid();

	// On the step
	glColor3f(175.0f / 255.0f, 127.0f / 255.0f, 113.0f / 255.0f);
	construct_cuboid(225.0f, 12.0f, 710.0f, 215.0f, 14.0f, 182.0f);
	build_cuboid();

	// Texture mapping for step
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_red);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(225.0f, 26.1f, 710.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(225.0f, 26.1f, 892.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(440.0f, 26.1f, 892.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(440.0f, 26.1f, 710.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// On the step -> gress (texture mapping)
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	construct_cuboid(240.0f, 14.0f, 720.0f, 185.0f, 15.0f, 145.0f);
	build_cuboid();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(240.0f, 30.0f, 720.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(240.0f, 30.0f, 865.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(425.0f, 30.0f, 865.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(425.0f, 30.0f, 720.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);


	// The foundation of Science Buildings
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(525.0f, 10.0f, -200.0f, 375.0f, 10.0f, 1100.0f);
	build_cuboid();
}


// Function to draw UFO
void draw_ufo()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 800.0f, 0.0f);
	glutSolidSphere(30.0, 20, 20);
	glPopMatrix();

	glColor3f(32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f);
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 770.0f);
	glutSolidCone(60.0, 60.0, 20, 20);
	glPopMatrix();
}


// Function to draw boat
void draw_boat()
{
	glColor3f(153.0f / 255.0f, 76.0f / 255.0f, 0.0f);
	construct_cuboid(-190.0f, 25.0f, -360.0f, 120.0f, 10.0f, 60.0f);
	build_cuboid();

	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(-190.0f, 35.0f, -360.0f, 120.0f, 5.0f, 60.0f);
	build_cuboid();

	glColor3f(0.0f, 0.0f, 192.0f / 255.0f);
	construct_cuboid(-190.0f, 40.0f, -360.0f, 120.0f, 10.0f, 10.0f);
	build_cuboid();

	construct_cuboid(-190.0f, 40.0f, -300.0f, 120.0f, 10.0f, -10.0f);
	build_cuboid();

	construct_cuboid(-190.0f, 40.0f, -360.0f, 10.0f, 10.0f, 60.0f);
	build_cuboid();

	construct_cuboid(-70.0f, 40.0f, -360.0f, -10.0f, 10.0f, 60.0f);
	build_cuboid();
}


// build a pool in the 3D scene
void draw_pool()
{
	// Water -> texture mapping
	glColor3f(141.0f / 255.0f, 193.0f / 255.0f, 241.0f / 255.0f);
	construct_cuboid(120.0f, 10.0f, -240.0f, -500.0f, 14.0f, -180.0f);
	build_cuboid();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_water);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-405.0f, 26.0f, -420.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-405.0f, 26.0f, -240.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(120.0f, 26.0f, -240.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(120.0f, 26.0f, -420.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Left wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(-380.0f, 10.0f, -240.0f, -25.0f, 22.0f, -180.0f);
	build_cuboid();

	// Right wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(145.0f, 10.0f, -240.0f, -25.0f, 22.0f, -180.0f);
	build_cuboid();

	// Top wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(-405.0f, 10.0f, -420.0f, 550.0f, 22.0f, -25.0f);
	build_cuboid();

	// Bottom wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(-405.0f, 10.0f, -220.0f, 550.0f, 22.0f, -25.0f);
	build_cuboid();

	// Draw boat
	glPushMatrix();
	glTranslatef(boat_x, boat_y, boat_z);
	draw_boat();
	glPopMatrix();

	// Draw UFO
	glPushMatrix();
	glTranslatef(ufo_x, ufo_y, ufo_z);
	draw_ufo();
	glPopMatrix();
}


// Draw the name of school shown on the Center Building
void draw_name()
{
	glColor3f(0.0f, 0.0f, 55.0f / 255.0f);
	selectFont(30, ANSI_CHARSET, "Helvetica");
	glRasterPos3f(180.0f, 720.0f, 150.0f);
	drawString(" Xi'an Jiaotong-Liverpool University");

	selectFont(45, GB2312_CHARSET, "华文行楷");
	glRasterPos3f(255.0f, 600.0f, 150.0f);
	drawString("西交利物浦大学");
}


// Function to draw Center Building
void draw_center_building()
{
	// Foundation
	glColor3f(160.0f / 255.0f, 160.0f / 255.0f, 160.0f / 255.0f);
	construct_cuboid(-450.0f, 10.0f, -200.0f, 595.0f, 10.0f, 800.0f);
	build_cuboid();

	// Foundation -> second floor -> texture mapping
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_trapezoid(-300.0f, 20.0f, 0.0f, -150.0f, 100.0f, -200.0f);
	build_trapezoid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_trapezoid(145.0f, 20.0f, 0.0f, -150.0f, 100.0f, -200.0f);
	build_trapezoid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(-450.0f, 20.0f, 0.0f, 500.0f, 100.0f, 450.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(-450.0f, 20.0f, 450.0f, 150.0f, 100.0f, 150.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(-45.0f, 20.0f, 450.0f, 190.0f, 100.0f, 150.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(50.0f, 20.0f, 0.0f, 50.0f, 100.0f, 130.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	construct_cuboid(50.0f, 20.0f, 280.0f, 50.0f, 100.0f, 170.0f);
	build_cuboid();


	// Texture mapping for foundation -> grass
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_grass);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(50.0f, 120.1f, 300.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(50.0f, 120.1f, 340.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0f, 120.1f, 340.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0f, 120.1f, 300.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(50.0f, 120.1f, 370.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(50.0f, 120.1f, 410.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0f, 120.1f, 410.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0f, 120.1f, 370.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(50.0f, 120.1f, 430.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(50.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0f, 120.1f, 430.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(50.0f, 120.1f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(50.0f, 120.1f, 20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0f, 120.1f, 20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0f, 120.1f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(50.0f, 120.1f, 50.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(50.0f, 120.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(100.0f, 120.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(100.0f, 120.1f, 50.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5.0f, 120.1f, -100.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-5.0f, 20.1f, -200.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(145.0f, 20.1f, -200.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(145.0f, 120.1f, -100.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5.0f, 120.1f, -50.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-5.0f, 120.1f, -100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(145.0f, 120.1f, -100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(145.0f, 120.1f, -50.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-45.0f, 120.1f, 450.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-45.0f, 120.1f, 495.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(145.0f, 120.1f, 495.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(145.0f, 120.1f, 450.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-45.0f, 120.1f, 520.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-45.0f, 120.1f, 550.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(145.0f, 120.1f, 550.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(145.0f, 120.1f, 520.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-45.0f, 120.1f, 575.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-45.0f, 120.1f, 600.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(145.0f, 120.1f, 600.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(145.0f, 120.1f, 575.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 590.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 600.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-300.0f, 120.1f, 600.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-300.0f, 120.1f, 590.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 510.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 570.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-300.0f, 120.1f, 570.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-300.0f, 120.1f, 510.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 490.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-300.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-300.0f, 120.1f, 490.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 440.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 450.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 440.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 400.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 400.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 300.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 360.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 360.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 300.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 270.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 270.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 200.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 170.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 110.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 110.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 170.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 80.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 50.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 50.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 80.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, 25.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-400.0f, 120.1f, 25.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-400.0f, 120.1f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, -50.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 120.1f, -100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-300.0f, 120.1f, -100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-300.0f, 120.1f, -50.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-450.0f, 120.1f, -100.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-450.0f, 20.1f, -200.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-300.0f, 20.1f, -200.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-300.0f, 120.1f, -100.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Foundation -> floor -> right
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);

	GLfloat x = 50.0f;
	for (GLfloat i = 90.0f; i > 0.0f; i -= 10.0f)
	{
		construct_cuboid(x, 20.0f, 130.0f, 10.0f, i, 150.0f);
		build_cuboid();

		x += 10.0f;
	}

	// Foundation -> floor -> top
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);

	GLfloat z = -15.0f;
	for (GLfloat i = 90.0f; i > 0.0f; i -= 10.0f)
	{
		construct_cuboid(-225.0f, 20.0f, z, 150.0f, i, 15.0f);
		build_cuboid();

		z -= 15.0f;
	}

	// Foundation -> floor -> bottom
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);

	z = 450.0f;
	for (GLfloat i = 90.0f; i > 0.0f; i -= 10.0f)
	{
		construct_cuboid(-250.0f, 20.0f, z, 150.0f, i, 15.0f);
		build_cuboid();

		z += 15.0f;
	}

	// Texture for each floor in building -> total 10 floors
	GLfloat y = 121.0f;
	for (int i = 0; i < 10; i++)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_floor);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-400.0f, y, 0.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-400.0f, y, 450.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(50.0f, y, 450.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(50.0f, y, 0.0f);

		glEnd();
		glDisable(GL_TEXTURE_2D);

		y += 35.0f;
	}


	// Texture mapping for roof of Center Building
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_roof_cb);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-400.0f, y + 1.1f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-400.0f, y + 1.1f, 450.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(50.0f, y + 1.1f, 450.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(50.0f, y + 1.1f, 0.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	
	// Building -> glass -> use glColor4f to adjust transparency
	// Blending is a technology that mixes the source color and the target color in a certain way to generate special effects. 
	// Blending is often used to draw transparent or semi-transparent objects.
	// Enable blend to draw a semi-transparent glass
	glEnable(GL_BLEND);

	// Semi-transparent blending function based on the alpha channel value of the source pixel
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor4f(51.0f / 255.0f, 51.0f / 255.0f, 1.0f, 0.35f);
	
	y = 120.0f;
	for (int i = 0; i < 10; i++)
	{
		construct_cuboid(-400.0f, y, 0.0f, 450.0f, 35.0f, 450.0f);
		build_cuboid();

		y += 35.0f;
	}

	// close the blend function
	glDisable(GL_BLEND);

	// Roof of Center Building
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(-400.0f, y, 0.0f, 450.0f, 2.0f, 450.0f);
	build_cuboid();


	// The body part for Center Building -> contains windows
	// Right
	glBegin(GL_LINES);
	glVertex3f(-400.0f, y + 20.0f, 0.0f);
	glVertex3f(-400.0f, 120.0f, 0.0f);

	glVertex3f(50.0f, y + 20.0f, 0.0f);
	glVertex3f(50.0f, 120.0f, 0.0f);

	glVertex3f(-400.0f, y + 20.0f, 450.0f);
	glVertex3f(-400.0f, 120.0f, 450.0f);

	glVertex3f(50.0f, y + 20.0f, 450.0f);
	glVertex3f(50.0f, 120.0f, 450.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(50.0f, y + 20.0f, 450.0f, 1.0f, -160.0f, -200.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = y + 20.f; i > y - 140.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, i, 450.0f);
		glVertex3f(52.0f, i, 250.0f);
		glEnd();
	}

	for (GLfloat i = 440.0f; i > 250.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, y + 20.0f, i);
		glVertex3f(52.0f, y - 140.0f, i);
		glEnd();
	}

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(52.0f, y - 140.0f, 450.0f);
	glVertex3f(52.0f, y - 140.0f, 250.0f);

	glVertex3f(52.0f, y + 20.0f, 250.0f);
	glVertex3f(52.0f, y - 140.0f, 250.0f);
	glEnd();


	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(50.0f, y + 20.0f, 200.0f, 1.0f, -180.0f, -200.0f);
	build_cuboid();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = y + 20.f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, i, 200.0f);
		glVertex3f(52.0f, i, 0.0f);
		glEnd();
	}

	for (GLfloat i = 190.0f; i > 0.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, y + 20.0f, i);
		glVertex3f(52.0f, y - 160.0f, i);
		glEnd();
	}

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(52.0f, y - 160.0f, 200.0f);
	glVertex3f(52.0f, y - 160.0f, 0.0f);

	glVertex3f(52.0f, y + 20.0f, 200.0f);
	glVertex3f(52.0f, y - 160.0f, 200.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(51.0f, 120.0f, 450.0f);
	glVertex3f(51.0f, 280.0f, 450.0f);
	glVertex3f(51.0f, 280.0f, 300.0f);
	glVertex3f(51.0f, 200.0f, 0.0f);
	glVertex3f(51.0f, 120.0f, 0.0f);
	glEnd();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = 270.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, i, 450.0f);
		glVertex3f(52.0f, i, 300.0f);
		glEnd();
	}

	for (GLfloat i = 200.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, i, 300.0f);
		glVertex3f(52.0f, i, 0.0f);
		glEnd();
	}

	GLfloat zz = 20.0f;
	for (GLfloat i = 270.0f; i > 200.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, i, 300.0f);
		glVertex3f(52.0f, i, 300.0f - zz);

		glVertex3f(52.0f, i, 300.0f - zz);
		glVertex3f(52.0f, 120.0f, 300.0f - zz);
		glEnd();
		zz += 38.0f;
	}

	glBegin(GL_LINES);
	glVertex3f(52.0f, 200.0f, 15.0f);
	glVertex3f(52.0f, 120.0f, 15.0f);
	glEnd();

	for (GLfloat i = 440.0f; i > 300.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, 280.0f, i);
		glVertex3f(52.0f, 120.0f, i);
		glEnd();
	}

	for (GLfloat i = 440.0f; i > 250.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(52.0f, y + 20.0f, i);
		glVertex3f(52.0f, y - 140.0f, i);
		glEnd();
	}

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(52.0f, 280.0f, 450.0f);
	glVertex3f(52.0f, 280.0f, 300.0f);

	glVertex3f(52.0f, 280.0f, 300.0f);
	glVertex3f(52.0f, 200.0f, 0.0f);
	glEnd();


	// Back
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(50.0f, 120.0f, -1.0f);
	glVertex3f(50.0f, 200.0f, -1.0f);
	glVertex3f(-250.0f, 260.0f, -1.0f);
	glVertex3f(-400.0f, 260.0f, -1.0f);
	glVertex3f(-400.0f, 120.0f, -1.0f);
	glEnd();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	GLfloat xx = 50.0f;
	for (GLfloat i = 250.f; i > 200.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-250.0f + xx, i, -3.0f);
		glVertex3f(-250.0f, i, -3.0f);

		glVertex3f(-250.0f + xx - 25.0f, i, -3.0f);
		glVertex3f(-250.0f + xx - 25.0f, 200.0f, -3.0f);

		glVertex3f(-250.0f + xx, i, -3.0f);
		glVertex3f(-250.0f + xx, 200.0f, -3.0f);
		glEnd();

		xx += 50.0f;
	}

	for (GLfloat i = 50.0f; i > -250.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 200.0f, -3.0f);
		glVertex3f(i, 120.0f, -3.0f);
		glEnd();
	}

	for (GLfloat i = 200.f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(50.0f, i, -3.0f);
		glVertex3f(-250.0f, i, -3.0f);
		glEnd();
	}

	for (GLfloat i = 250.f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-250.0f, i, -3.0f);
		glVertex3f(-400.0f, i, -3.0f);
		glEnd();
	}

	for (GLfloat i = -250.0f; i > -400.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 260.0f, -3.0f);
		glVertex3f(i, 120.0f, -3.0f);
		glEnd();
	}


	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(50.0f, y + 20.0f, -1.0f);
	glVertex3f(0.0f, y + 20.0f, -1.0f);
	glVertex3f(-50.0f, y - 70.0f, -1.0f);
	glVertex3f(-50.0f, y - 160.0f, -1.0f);
	glVertex3f(50.0f, y - 160.0f, -1.0f);
	glEnd();

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(50.0f, 200, -3.0f);
	glVertex3f(-250.0f, 260.0f, -3.0f);

	glVertex3f(-250.0f, 260.0f, -3.0f);
	glVertex3f(-400.0f, 260.0f, -3.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-50.0f, y - 70.0f, -1.0f);
	glVertex3f(-50.0f, y - 160.0f, -1.0f);
	glVertex3f(-150.0f, y - 160.0f, -1.0f);
	glVertex3f(-220.0f, y - 90.0f, -1.0f);
	glVertex3f(-80.0f, y - 70.0f, -1.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-80.0f, y - 70.0f, -1.0f);
	glVertex3f(-220.0f, y - 90.0f, -1.0f);
	glVertex3f(-300.0f, y - 90.0f, -1.0f);
	glVertex3f(-300.0f, y + 20.0f, -1.0f);
	glVertex3f(-80.0f, y + 20.0f, -1.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-300.0f, y + 20.0f, -1.0f);
	glVertex3f(-300.0f, y - 90.0f, -1.0f);
	glVertex3f(-350.0f, y - 160.0f, -1.0f);
	glVertex3f(-400.0f, y - 160.0f, -1.0f);
	glVertex3f(-400.0f, y + 20.0f, -1.0f);
	glEnd();

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, y + 20.0f, -3.0f);
	glVertex3f(-50.0f, y - 70.0f, -3.0f);

	glVertex3f(-50.0f, y - 70.0f, -3.0f);
	glVertex3f(-80.0f, y - 70.0f, -3.0f);

	glVertex3f(-80.0f, y + 20.0f, -3.0f);
	glVertex3f(-80.0f, y - 70.0f, -3.0f);

	glVertex3f(50.0f, y - 160.0f, -3.0f);
	glVertex3f(-150.0f, y - 160.0f, -3.0f);

	glVertex3f(-150.0f, y - 160.0f, -3.0f);
	glVertex3f(-220.0f, y - 90.0f, -3.0f);

	glVertex3f(-220.0f, y - 90.0f, -3.0f);
	glVertex3f(-300.0f, y - 90.0f, -3.0f);

	glVertex3f(-300.0f, y - 90.0f, -3.0f);
	glVertex3f(-350.0f, y - 160.0f, -3.0f);

	glVertex3f(-350.0f, y - 160.0f, -3.0f);
	glVertex3f(-400.0f, y - 160.0f, -3.0f);
	glEnd();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = 50.0f; i >= -150.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y - 90.0f, -3.0f);
		glVertex3f(i, y - 160.0f, -3.0f);
		glEnd();
	}

	for (GLfloat i = y - 70.0f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(50.0f, i, -3.0f);
		glVertex3f(-150.0f, i, -3.0f);
		glEnd();
	}

	for (GLfloat i = -90.0f; i > -400.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y + 20.0f, -3.0f);
		glVertex3f(i, y - 90.0f, -3.0f);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > y - 90.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-80.0f, i, -3.0f);
		glVertex3f(-400.0f, i, -3.0f);
		glEnd();
	}

	for (GLfloat i = 50.0f; i >= 0.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y + 20.0f, -3.0f);
		glVertex3f(i, y - 90.0f, -3.0f);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > y - 70.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(50.0f, i, -3.0f);
		glVertex3f(0.0f, i, -3.0f);
		glEnd();
	}

	glBegin(GL_LINES);
	glVertex3f(-25.0f, y - 30.0f, -3.0f);
	glVertex3f(-25.0f, y - 90.0f, -3.0f);

	glVertex3f(0.0f, y - 10.0f, -3.0f);
	glVertex3f(-15.0f, y - 10.0f, -3.0f);

	glVertex3f(0.0f, y - 20.0f, -3.0f);
	glVertex3f(-20.0f, y - 20.0f, -3.0f);

	glVertex3f(0.0f, y - 30.0f, -3.0f);
	glVertex3f(-25.0f, y - 30.0f, -3.0f);

	glVertex3f(0.0f, y - 40.0f, -3.0f);
	glVertex3f(-30.0f, y - 40.0f, -3.0f);

	glVertex3f(0.0f, y - 50.0f, -3.0f);
	glVertex3f(-40.0f, y - 50.0f, -3.0f);

	glVertex3f(0.0f, y - 60.0f, -3.0f);
	glVertex3f(-45.0f, y - 60.0f, -3.0f);

	glVertex3f(-50.0f, y - 70.0f, -3.0f);
	glVertex3f(-50.0f, y - 90.0f, -3.0f);

	glVertex3f(-150.0f, y - 90.0f, -3.0f);
	glVertex3f(-220.0f, y - 90.0f, -3.0f);

	glVertex3f(-300.0f, y - 90.0f, -3.0f);
	glVertex3f(-400.0f, y - 90.0f, -3.0f);

	glVertex3f(-150.0f, y - 100.0f, -3.0f);
	glVertex3f(-205.0f, y - 100.0f, -3.0f);

	glVertex3f(-150.0f, y - 110.0f, -3.0f);
	glVertex3f(-195.0f, y - 110.0f, -3.0f);

	glVertex3f(-150.0f, y - 120.0f, -3.0f);
	glVertex3f(-185.0f, y - 120.0f, -3.0f);

	glVertex3f(-150.0f, y - 130.0f, -3.0f);
	glVertex3f(-177.0f, y - 130.0f, -3.0f);

	glVertex3f(-150.0f, y - 140.0f, -3.0f);
	glVertex3f(-167.0f, y - 140.0f, -3.0f);

	glVertex3f(-175.0f, y - 130.0f, -3.0f);
	glVertex3f(-175.0f, y - 90.0f, -3.0f);

	glVertex3f(-200.0f, y - 90.0f, -3.0f);
	glVertex3f(-200.0f, y - 110.0f, -3.0f);

	glVertex3f(-308.0f, y - 100.0f, -3.0f);
	glVertex3f(-400.0f, y - 100.0f, -3.0f);

	glVertex3f(-315.0f, y - 110.0f, -3.0f);
	glVertex3f(-400.0f, y - 110.0f, -3.0f);

	glVertex3f(-325.0f, y - 120.0f, -3.0f);
	glVertex3f(-400.0f, y - 120.0f, -3.0f);

	glVertex3f(-330.0f, y - 130.0f, -3.0f);
	glVertex3f(-400.0f, y - 130.0f, -3.0f);

	glVertex3f(-340.0f, y - 140.0f, -3.0f);
	glVertex3f(-400.0f, y - 140.0f, -3.0f);

	glVertex3f(-345.0f, y - 150.0f, -3.0f);
	glVertex3f(-400.0f, y - 150.0f, -3.0f);

	glVertex3f(-390.0f, y - 90.0f, -3.0f);
	glVertex3f(-390.0f, y - 160.0f, -3.0f);

	glVertex3f(-365.0f, y - 90.0f, -3.0f);
	glVertex3f(-365.0f, y - 160.0f, -3.0f);

	glVertex3f(-340.0f, y - 90.0f, -3.0f);
	glVertex3f(-340.0f, y - 140.0f, -3.0f);

	glVertex3f(-315.0f, y - 90.0f, -3.0f);
	glVertex3f(-315.0f, y - 105.0f, -3.0f);
	glEnd();

	// Front
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(50.0f, y + 20.0f, 451.0f);
	glVertex3f(50.0f, y - 140.0f, 451.0f);
	glVertex3f(0.0f, y - 160.0f, 451.0f);
	glVertex3f(-200.0f, y - 160.0f, 451.0f);
	glVertex3f(-200.0f, y + 20.0f, 451.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-200.0f, y - 160.0f, 451.0f);
	glVertex3f(-200.0f, y - 60.0f, 451.0f);
	glVertex3f(-250.0f, y - 60.0f, 451.0f);
	glVertex3f(-300.0f, y + 20.0f, 451.0f);
	glVertex3f(-400.0f, y + 20.0f, 451.0f);
	glVertex3f(-400.0f, y - 160.0f, 451.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(50.0f, 280.0f, 451.0f);
	glVertex3f(50.0f, 120.0f, 451.0f);
	glVertex3f(-300.0f, 120.0f, 451.0f);
	glVertex3f(-300.0f, 240.0f, 451.0f);
	glVertex3f(-200.0f, 280.0f, 451.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-300.0f, 120.0f, 451.0f);
	glVertex3f(-300.0f, 240.0f, 451.0f);
	glVertex3f(-400.0f, 240.0f, 451.0f);
	glVertex3f(-400.0f, 120.0f, 451.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-300.0f, 240.0f, 451.0f);
	glVertex3f(-300.0f, y - 160.0f, 451.0f);
	glVertex3f(-380.0f, y - 160.0f, 451.0f);
	glVertex3f(-360.0f, 240.0f, 451.0f);
	glEnd();

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(50.0f, y - 140.0f, 453.0f);
	glVertex3f(0.0f, y - 160.0f, 453.0f);

	glVertex3f(0.0f, y - 160.0f, 453.0f);
	glVertex3f(-300.0f, y - 160.0f, 453.0f);

	glVertex3f(50.0f, 280.0f, 453.0f);
	glVertex3f(-200.0f, 280.0f, 453.0f);

	glVertex3f(-200.0f, 280.0f, 453.0f);
	glVertex3f(-300.0f, 240.0f, 453.0f);

	glVertex3f(-300.0f, y - 160.0f, 453.0f);
	glVertex3f(-300.0f, 240.0f, 453.0f);

	glVertex3f(-380.0f, y - 160.0f, 453.0f);
	glVertex3f(-360.0f, 240.0f, 453.0f);

	glVertex3f(-360.0f, 240.0f, 453.0f);
	glVertex3f(-400.0f, 240.0f, 453.0f);

	glVertex3f(-380.0f, y - 160.0f, 453.0f);
	glVertex3f(-400.0f, y - 160.0f, 453.0f);

	glVertex3f(-200.0f, y + 20.0f, 453.0f);
	glVertex3f(-200.0f, y - 60.0f, 453.0f);

	glVertex3f(-200.0f, y - 60.0f, 453.0f);
	glVertex3f(-250.0f, y - 60.0f, 453.0f);

	glVertex3f(-300.0f, y + 20.0f, 453.0f);
	glVertex3f(-250.0f, y - 60.0f, 453.0f);
	glEnd();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = 50.0f; i > 0.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y + 20.0f, 453.0f);
		glVertex3f(i, y - 140.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > y - 140.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(50.0f, i, 453.0f);
		glVertex3f(0.0f, i, 453.0f);
		glEnd();
	}

	for (GLfloat i = 0.0f; i > -200.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y + 20.0f, 453.0f);
		glVertex3f(i, y - 160.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(0.0f, i, 453.0f);
		glVertex3f(-200.0f, i, 453.0f);
		glEnd();
	}

	for (GLfloat i = 50.0f; i >= -200.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 120.0f, 453.0f);
		glVertex3f(i, 280.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = 280.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(50.0f, i, 453.0f);
		glVertex3f(-200.0f, i, 453.0f);
		glEnd();
	}

	for (GLfloat i = -200.0f; i > -400.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, 240.0f, 453.0f);
		glVertex3f(i, 120.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = 240.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-200.0f, i, 453.0f);
		glVertex3f(-400.0f, i, 453.0f);
		glEnd();
	}

	for (GLfloat i = -200.0f; i >= -300.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y - 60.0f, 453.0f);
		glVertex3f(i, y - 160.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = y - 60.0f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-200.0f, i, 453.0f);
		glVertex3f(-300.0f, i, 453.0f);
		glEnd();
	}

	for (GLfloat i = -300.0f; i > -400.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(i, y + 20.0f, 453.0f);
		glVertex3f(i, y - 160.0f, 453.0f);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-300.0f, i, 453.0f);
		glVertex3f(-400.0f, i, 453.0f);
		glEnd();
	}

	glBegin(GL_LINES);
	glVertex3f(45.0f, y - 140.0f, 453.0f);
	glVertex3f(0.0f, y - 140.0f, 453.0f);

	glVertex3f(20.0f, y - 150.0f, 453.0f);
	glVertex3f(0.0f, y - 150.0f, 453.0f);

	glVertex3f(25.0f, y - 140.0f, 453.0f);
	glVertex3f(25.0f, y - 150.0f, 453.0f);

	glVertex3f(-200.0f, 270.0f, 453.0f);
	glVertex3f(-220.0f, 270.0f, 453.0f);

	glVertex3f(-200.0f, 260.0f, 453.0f);
	glVertex3f(-245.0f, 260.0f, 453.0f);

	glVertex3f(-200.0f, 250.0f, 453.0f);
	glVertex3f(-270.0f, 250.0f, 453.0f);

	glVertex3f(-225.0f, 268.0f, 453.0f);
	glVertex3f(-225.0f, 120.0f, 453.0f);

	glVertex3f(-250.0f, 260.0f, 453.0f);
	glVertex3f(-250.0f, 120.0f, 453.0f);

	glVertex3f(-275.0f, 250.0f, 453.0f);
	glVertex3f(-275.0f, 120.0f, 453.0f);

	glVertex3f(-325.0f, y - 160.0f, 453.0f);
	glVertex3f(-325.0f, 240.0f, 453.0f);

	glVertex3f(-350.0f, y - 160.0f, 453.0f);
	glVertex3f(-350.0f, 240.0f, 453.0f);

	glVertex3f(-300.0f, y - 160.0f, 453.0f);
	glVertex3f(-380.0f, y - 160.0f, 453.0f);

	glVertex3f(-300.0f, y - 170.0f, 453.0f);
	glVertex3f(-375.0f, y - 170.0f, 453.0f);

	glVertex3f(-300.0f, y - 180.0f, 453.0f);
	glVertex3f(-370.0f, y - 180.0f, 453.0f);

	glVertex3f(-300.0f, y - 190.0f, 453.0f);
	glVertex3f(-368.0f, y - 190.0f, 453.0f);

	glVertex3f(-300.0f, y - 200.0f, 453.0f);
	glVertex3f(-366.0f, y - 200.0f, 453.0f);

	glVertex3f(-300.0f, y - 210.0f, 453.0f);
	glVertex3f(-364.0f, y - 210.0f, 453.0f);

	glVertex3f(-300.0f, y - 220.0f, 453.0f);
	glVertex3f(-362.0f, y - 220.0f, 453.0f);

	glVertex3f(-275.0f, y - 60.0f, 453.0f);
	glVertex3f(-275.0f, y - 20.0f, 453.0f);

	glVertex3f(-300.0f, y + 10.0f, 453.0f);
	glVertex3f(-295.0f, y + 10.0f, 453.0f);

	glVertex3f(-300.0f, y, 453.0f);
	glVertex3f(-290.0f, y, 453.0f);

	glVertex3f(-300.0f, y - 10.0f, 453.0f);
	glVertex3f(-285.0f, y - 10.0f, 453.0f);

	glVertex3f(-300.0f, y - 20.0f, 453.0f);
	glVertex3f(-275.0f, y - 20.0f, 453.0f);

	glVertex3f(-300.0f, y - 30.0f, 453.0f);
	glVertex3f(-270.0f, y - 30.0f, 453.0f);

	glVertex3f(-300.0f, y - 40.0f, 453.0f);
	glVertex3f(-265.0f, y - 40.0f, 453.0f);

	glVertex3f(-300.0f, y - 50.0f, 453.0f);
	glVertex3f(-255.0f, y - 50.0f, 453.0f);

	glEnd();

	// Left
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-401.0f, 120.0f, 250.0f);
	glVertex3f(-401.0f, 260.0f, 200.0f);
	glVertex3f(-401.0f, 260.0f, 0.0f);
	glVertex3f(-401.0f, 120.0f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-401.0f, 310.0f, 0.0f);
	glVertex3f(-401.0f, y + 20.0f, 0.0f);
	glVertex3f(-401.0f, y + 20.0f, 200.0f);
	glVertex3f(-401.0f, y - 70.0f, 240.0f);
	glVertex3f(-401.0f, 310.0f, 200.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-401.0f, y + 20.0f, 450.0f);
	glVertex3f(-401.0f, y - 160.0f, 450.0f);
	glVertex3f(-401.0f, y - 160.0f, 425.0f);
	glVertex3f(-401.0f, y - 70.0f, 300.0f);
	glVertex3f(-401.0f, y + 20.0f, 300.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-401.0f, y - 70.0f, 300.0f);
	glVertex3f(-401.0f, 120.0f, 350.0f);
	glVertex3f(-401.0f, 240.0f, 400.0f);
	glVertex3f(-401.0f, y - 160.0f, 425.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-401.0f, 120.0f, 350.0f);
	glVertex3f(-401.0f, 120.0f, 450.0f);
	glVertex3f(-401.0f, 240.0f, 450.0f);
	glVertex3f(-401.0f, 240.0f, 400.0f);
	glEnd();

	glColor3f(153.0f / 255.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-403.0f, 240.0f, 450.0f);
	glVertex3f(-403.0f, 240.0f, 400.0f);

	glVertex3f(-403.0f, y - 160.0f, 450.0f);
	glVertex3f(-403.0f, y - 160.0f, 425.0f);

	glVertex3f(-403.0f, 240.0f, 400.0f);
	glVertex3f(-403.0f, y - 160.0f, 425.0f);

	glVertex3f(-403.0f, 120.0f, 350.0f);
	glVertex3f(-403.0f, y - 70.0f, 300.0f);

	glVertex3f(-403.0f, y - 70.0f, 300.0f);
	glVertex3f(-403.0f, y + 20.0f, 300.0f);

	glVertex3f(-403.0f, 120.0f, 250.0f);
	glVertex3f(-403.0f, 260.0f, 200.0f);

	glVertex3f(-403.0f, 260.0f, 200.0f);
	glVertex3f(-403.0f, 260.0f, 0.0f);

	glVertex3f(-403.0f, 310.0f, 200.0f);
	glVertex3f(-403.0f, 310.0f, 0.0f);

	glVertex3f(-403.0f, 310.0f, 200.0f);
	glVertex3f(-403.0f, y - 70.0f, 240.0f);

	glVertex3f(-403.0f, y - 70.0f, 240.0f);
	glVertex3f(-403.0f, y + 20.0f, 200.0f);
	glEnd();

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	for (GLfloat i = 450.0f; i > 300.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, y + 20.0f, i);
		glVertex3f(-403.0f, y - 70.0f, i);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i >= y - 70.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, i, 450.0f);
		glVertex3f(-403.0f, i, 300.0f);
		glEnd();
	}

	for (GLfloat i = 200.0f; i > 0.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, y + 20.0f, i);
		glVertex3f(-403.0f, 310.0f, i);
		glEnd();
	}

	for (GLfloat i = y + 20.0f; i > 310.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, i, 200.0f);
		glVertex3f(-403.0f, i, 0.0f);
		glEnd();
	}

	for (GLfloat i = 200.0f; i > 0.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, 260.0f, i);
		glVertex3f(-403.0f, 120.0f, i);
		glEnd();
	}

	for (GLfloat i = 260.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, i, 200.0f);
		glVertex3f(-403.0f, i, 0.0f);
		glEnd();
	}

	for (GLfloat i = 450.0f; i >= 350.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, 240.0f, i);
		glVertex3f(-403.0f, 120.0f, i);
		glEnd();
	}

	for (GLfloat i = 240.0f; i > 120.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, i, 450.0f);
		glVertex3f(-403.0f, i, 350.0f);
		glEnd();
	}

	for (GLfloat i = 450.0f; i >= 350.0f; i -= 25.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, y - 70.0f, i);
		glVertex3f(-403.0f, y - 160.0f, i);
		glEnd();
	}

	for (GLfloat i = y - 70.0f; i > y - 160.0f; i -= 10.0f)
	{
		glBegin(GL_LINES);
		glVertex3f(-403.0f, i, 450.0f);
		glVertex3f(-403.0f, i, 350.0f);
		glEnd();
	}

	glBegin(GL_LINES);
	glVertex3f(-403.0f, y - 80.0f, 350.0f);
	glVertex3f(-403.0f, y - 80.0f, 300.0f);

	glVertex3f(-403.0f, y - 90.0f, 350.0f);
	glVertex3f(-403.0f, y - 90.0f, 303.0f);

	glVertex3f(-403.0f, y - 100.0f, 350.0f);
	glVertex3f(-403.0f, y - 100.0f, 304.0f);

	glVertex3f(-403.0f, y - 110.0f, 350.0f);
	glVertex3f(-403.0f, y - 110.0f, 305.0f);

	glVertex3f(-403.0f, y - 120.0f, 350.0f);
	glVertex3f(-403.0f, y - 120.0f, 307.0f);

	glVertex3f(-403.0f, y - 130.0f, 350.0f);
	glVertex3f(-403.0f, y - 130.0f, 310.0f);

	glVertex3f(-403.0f, y - 140.0f, 350.0f);
	glVertex3f(-403.0f, y - 140.0f, 315.0f);

	glVertex3f(-403.0f, y - 150.0f, 350.0f);
	glVertex3f(-403.0f, y - 150.0f, 318.0f);

	glVertex3f(-403.0f, y - 160.0f, 425.0f);
	glVertex3f(-403.0f, y - 160.0f, 319.0f);

	glVertex3f(-403.0f, y - 160.0f, 325.0f);
	glVertex3f(-403.0f, y - 70.0f, 325.0f);

	glVertex3f(-403.0f, y - 170.0f, 422.0f);
	glVertex3f(-403.0f, y - 170.0f, 320.0f);

	glVertex3f(-403.0f, y - 180.0f, 420.0f);
	glVertex3f(-403.0f, y - 180.0f, 322.0f);

	glVertex3f(-403.0f, y - 190.0f, 416.0f);
	glVertex3f(-403.0f, y - 190.0f, 324.0f);

	glVertex3f(-403.0f, y - 200.0f, 412.0f);
	glVertex3f(-403.0f, y - 200.0f, 326.0f);

	glVertex3f(-403.0f, y - 210.0f, 406.0f);
	glVertex3f(-403.0f, y - 210.0f, 328.0f);

	glVertex3f(-403.0f, y - 220.0f, 400.0f);
	glVertex3f(-403.0f, y - 220.0f, 331.0f);

	glVertex3f(-403.0f, y - 230.0f, 350.0f);
	glVertex3f(-403.0f, y - 230.0f, 331.0f);

	glVertex3f(-403.0f, y - 230.0f, 400.0f);
	glVertex3f(-403.0f, y - 160.0f, 400.0f);

	glVertex3f(-403.0f, y - 230.0f, 375.0f);
	glVertex3f(-403.0f, y - 160.0f, 375.0f);

	glVertex3f(-403.0f, y - 230.0f, 350.0f);
	glVertex3f(-403.0f, y - 160.0f, 350.0f);

	glVertex3f(-403.0f, y - 190.0f, 325.0f);
	glVertex3f(-403.0f, y - 160.0f, 325.0f);

	glVertex3f(-403.0f, y - 240.0f, 350.0f);
	glVertex3f(-403.0f, y - 240.0f, 333.0f);

	glVertex3f(-403.0f, y - 250.0f, 350.0f);
	glVertex3f(-403.0f, y - 250.0f, 334.0f);

	glVertex3f(-403.0f, y - 260.0f, 350.0f);
	glVertex3f(-403.0f, y - 260.0f, 335.0f);

	glVertex3f(-403.0f, y - 270.0f, 350.0f);
	glVertex3f(-403.0f, y - 270.0f, 336.0f);

	glVertex3f(-403.0f, y - 280.0f, 350.0f);
	glVertex3f(-403.0f, y - 280.0f, 338.0f);

	glVertex3f(-403.0f, y - 290.0f, 350.0f);
	glVertex3f(-403.0f, y - 290.0f, 340.0f);

	glVertex3f(-403.0f, y - 300.0f, 350.0f);
	glVertex3f(-403.0f, y - 300.0f, 342.0f);

	glVertex3f(-403.0f, y - 310.0f, 350.0f);
	glVertex3f(-403.0f, y - 310.0f, 344.0f);

	glVertex3f(-403.0f, y, 200.0f);
	glVertex3f(-403.0f, y, 205.0f);

	glVertex3f(-403.0f, y - 10.0f, 200.0f);
	glVertex3f(-403.0f, y - 10.0f, 210.0f);

	glVertex3f(-403.0f, y - 20.0f, 200.0f);
	glVertex3f(-403.0f, y - 20.0f, 217.0f);

	glVertex3f(-403.0f, y - 30.0f, 200.0f);
	glVertex3f(-403.0f, y - 30.0f, 220.0f);

	glVertex3f(-403.0f, y - 40.0f, 200.0f);
	glVertex3f(-403.0f, y - 40.0f, 224.0f);

	glVertex3f(-403.0f, y - 50.0f, 200.0f);
	glVertex3f(-403.0f, y - 50.0f, 228.0f);

	glVertex3f(-403.0f, y - 60.0f, 200.0f);
	glVertex3f(-403.0f, y - 60.0f, 232.0f);

	glVertex3f(-403.0f, y - 70.0f, 200.0f);
	glVertex3f(-403.0f, y - 70.0f, 240.0f);

	glVertex3f(-403.0f, y - 80.0f, 200.0f);
	glVertex3f(-403.0f, y - 80.0f, 232.0f);

	glVertex3f(-403.0f, y - 90.0f, 200.0f);
	glVertex3f(-403.0f, y - 90.0f, 228.0f);

	glVertex3f(-403.0f, y - 100.0f, 200.0f);
	glVertex3f(-403.0f, y - 100.0f, 224.0f);

	glVertex3f(-403.0f, y - 110.0f, 200.0f);
	glVertex3f(-403.0f, y - 110.0f, 220.0f);

	glVertex3f(-403.0f, y - 120.0f, 200.0f);
	glVertex3f(-403.0f, y - 120.0f, 217.0f);

	glVertex3f(-403.0f, y - 130.0f, 200.0f);
	glVertex3f(-403.0f, y - 130.0f, 210.0f);

	glVertex3f(-403.0f, y - 140.0f, 200.0f);
	glVertex3f(-403.0f, y - 140.0f, 207.0f);

	glVertex3f(-403.0f, y - 100.0f, 225.0f);
	glVertex3f(-403.0f, y - 40.0f, 225.0f);

	glVertex3f(-403.0f, 120.0f, 225.0f);
	glVertex3f(-403.0f, 190.0f, 225.0f);

	glVertex3f(-403.0f, 250.0f, 200.0f);
	glVertex3f(-403.0f, 250.0f, 203.0f);

	glVertex3f(-403.0f, 240.0f, 200.0f);
	glVertex3f(-403.0f, 240.0f, 206.0f);

	glVertex3f(-403.0f, 230.0f, 200.0f);
	glVertex3f(-403.0f, 230.0f, 208.0f);

	glVertex3f(-403.0f, 220.0f, 200.0f);
	glVertex3f(-403.0f, 220.0f, 211.0f);

	glVertex3f(-403.0f, 210.0f, 200.0f);
	glVertex3f(-403.0f, 210.0f, 215.0f);

	glVertex3f(-403.0f, 200.0f, 200.0f);
	glVertex3f(-403.0f, 200.0f, 218.0f);

	glVertex3f(-403.0f, 190.0f, 200.0f);
	glVertex3f(-403.0f, 190.0f, 223.0f);

	glVertex3f(-403.0f, 180.0f, 200.0f);
	glVertex3f(-403.0f, 180.0f, 226.0f);

	glVertex3f(-403.0f, 170.0f, 200.0f);
	glVertex3f(-403.0f, 170.0f, 230.0f);

	glVertex3f(-403.0f, 160.0f, 200.0f);
	glVertex3f(-403.0f, 160.0f, 234.0f);

	glVertex3f(-403.0f, 150.0f, 200.0f);
	glVertex3f(-403.0f, 150.0f, 238.0f);

	glVertex3f(-403.0f, 140.0f, 200.0f);
	glVertex3f(-403.0f, 140.0f, 244.0f);

	glVertex3f(-403.0f, 130.0f, 200.0f);
	glVertex3f(-403.0f, 130.0f, 248.0f);

	glEnd();
}


// Function to draw Science Buildings
void draw_science_building()
{
	// Texture mapping for buildings' floors
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_floor);

	GLfloat y = 20.1f;
	for (int i = 0; i < 5; i++)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(525.0f, y, -200.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(525.0f, y, -20.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(900.0f, y, -20.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(900.0f, y, -200.0f);

		glEnd();

		y += 40.0f;
	}

	glDisable(GL_TEXTURE_2D);

	// Texture mapping for buildings' windows
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_window);

	// Front -> first floor
	GLfloat x = 525.0f;

	for (x = 525.0f; x <= 850.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 60.0f, -200.1f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 20.0f, -200.1f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 20.0f, -200.1f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 60.0f, -200.1f);

		glEnd();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(880.0f, 60.0f, -200.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(880.0f, 20.0f, -200.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 20.0f, -200.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 60.0f, -200.1f);

	glEnd();

	// Front -> third floor
	x = 525.0f;

	for (x = 525.0f; x <= 850.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 140.0f, -200.1f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 100.0f, -200.1f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 100.0f, -200.1f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 140.0f, -200.1f);

		glEnd();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(880.0f, 140.0f, -200.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(880.0f, 100.0f, -200.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 100.0f, -200.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 140.0f, -200.1f);

	glEnd();

	// Front -> fifth floor
	x = 525.0f;

	for (x = 525.0f; x <= 850.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 220.0f, -200.1f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 180.0f, -200.1f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 180.0f, -200.1f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 220.0f, -200.1f);

		glEnd();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(880.0f, 220.0f, -200.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(880.0f, 180.0f, -200.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 180.0f, -200.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 220.0f, -200.1f);

	glEnd();

	// Front -> second floor
	x = 570.0f;

	for (x = 570.0f; x < 900.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 100.0f, -200.1f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 60.0f, -200.1f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 60.0f, -200.1f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 100.0f, -200.1f);

		glEnd();
	}

	// Front -> fourth floor
	x = 570.0f;

	for (x = 570.0f; x < 900.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 180.0f, -200.1f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 140.0f, -200.1f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 140.0f, -200.1f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 180.0f, -200.1f);

		glEnd();
	}

	// Back -> first floor
	x = 570.0f;

	for (x = 570.0f; x < 900.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 60.0f, -19.9f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 20.0f, -19.9f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 20.0f, -19.9f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 60.0f, -19.9f);

		glEnd();
	}

	// Back -> Third floor
	x = 570.0f;

	for (x = 570.0f; x < 900.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 140.0f, -19.9f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 100.0f, -19.9f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 100.0f, -19.9f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 140.0f, -19.9f);

		glEnd();
	}

	// Back -> fifth floor
	x = 570.0f;

	for (x = 570.0f; x < 900.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 220.0f, -19.9f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 180.0f, -19.9f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 180.0f, -19.9f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 220.0f, -19.9f);

		glEnd();
	}

	// Back -> second floor
	x = 525.0f;

	for (x = 525.0f; x <= 850.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 100.0f, -19.9f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 60.0f, -19.9f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 60.0f, -19.9f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 100.0f, -19.9f);

		glEnd();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(880.0f, 100.0f, -19.9f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(880.0f, 60.0f, -19.9f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 60.0f, -19.9f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 100.0f, -19.9f);

	glEnd();

	// Back -> fourth floor
	x = 525.0f;

	for (x = 525.0f; x <= 850.0f; x += 90.0f)
	{
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, 180.0f, -19.9f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, 140.0f, -19.9f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + 45.0f, 140.0f, -19.9f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + 45.0f, 180.0f, -19.9f);

		glEnd();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(880.0f, 180.0f, -19.9f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(880.0f, 140.0f, -19.9f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 140.0f, -19.9f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 180.0f, -19.9f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	
	// Texture mapping for buildings' wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_wall);

	// left wall
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(524.0f, 10.0f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(524.0f, 10.0f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(524.0f, 240.0f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(524.0f, 240.0f, -200.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(526.0f, 220.0f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(526.0f, 220.0f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(526.0f, 240.0f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(526.0f, 240.0f, -200.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(524.0f, 240.1f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(524.0f, 240.1f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(526.0f, 240.1f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(526.0f, 240.1f, -200.0f);

	glEnd();


	// Right wall
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(901.0f, 10.0f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(901.0f, 10.0f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(901.0f, 240.0f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(901.0f, 240.0f, -200.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(899.0f, 220.0f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(899.0f, 220.0f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(899.0f, 240.0f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(899.0f, 240.0f, -200.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(899.0f, 240.1f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(899.0f, 240.1f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(901.0f, 240.1f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(901.0f, 240.1f, -200.0f);

	glEnd();


	// Roof
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(525.0f, 221.1f, -200.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(525.0f, 221.1f, -20.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 221.1f, -20.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 221.1f, -200.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Blend function for glass
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor4f(0.0f, 0.0f, 1.0f, 0.35f);

	y = 20.0f;
	for (int i = 0; i < 5; i++)
	{
		construct_cuboid(525.0f, y, -200.0f, 375.0f, 40.0f, 180.0f);
		build_cuboid();

		y += 40.0f;
	}

	glDisable(GL_BLEND);

	// Roof of building
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(525.0f, y, -200.0f, 375.0f, 1.0f, 180.0f);
	build_cuboid();

	// Body of building
	glColor3f(1.0f, 153.0f / 255.0f, 51.0f / 255.0f);

	glBegin(GL_LINES);
	glVertex3f(525.0f, 20.0f, -200.0f);
	glVertex3f(525.0f, y + 20.0f, -200.0f);

	glVertex3f(900.0f, 20.0f, -200.0f);
	glVertex3f(900.0f, y + 20.0f, -200.0f);

	glVertex3f(525.0f, 20.0f, -20.0f);
	glVertex3f(525.0f, y + 20.0f, -20.0f);

	glVertex3f(900.0f, 20.0f, -20.0f);
	glVertex3f(900.0f, y + 20.0f, -20.0f);

	glEnd();

	// Body -> left wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(525.0f, 20.0f, -200.0f, -0.1f, y, 180.0f);
	build_cuboid();

	// Body -> right wall
	glColor3f(1.0f, 1.0f, 1.0f);
	construct_cuboid(900.0f, 20.0f, -200.0f, 0.1f, y, 180.0f);
	build_cuboid();

	// Body -> windows
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	// Back
	glVertex3f(525.0f, 60.0f, -200.1f);
	glVertex3f(900.0f, 60.0f, -200.1f);

	glVertex3f(525.0f, 100.0f, -200.1f);
	glVertex3f(900.0f, 100.0f, -200.1f);

	glVertex3f(525.0f, 140.0f, -200.1f);
	glVertex3f(900.0f, 140.0f, -200.1f);

	glVertex3f(525.0f, 180.0f, -200.1f);
	glVertex3f(900.0f, 180.0f, -200.1f);

	// Front
	glVertex3f(525.0f, 60.0f, -19.9f);
	glVertex3f(900.0f, 60.0f, -19.9f);

	glVertex3f(525.0f, 100.0f, -19.9f);
	glVertex3f(900.0f, 100.0f, -19.9f);

	glVertex3f(525.0f, 140.0f, -19.9f);
	glVertex3f(900.0f, 140.0f, -19.9f);

	glVertex3f(525.0f, 180.0f, -19.9f);
	glVertex3f(900.0f, 180.0f, -19.9f);
	glEnd();
}


// Function to draw the connection between the two buildings
void draw_connection()
{
	// Texture mapping for ground
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_red);

	// Between SA and SB
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(525.0f, 20.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(525.0f, 20.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 20.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 20.1f, -20.0f);

	glEnd();

	// Between SB and SC
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(525.0f, 20.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(525.0f, 20.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 20.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 20.1f, 280.0f);

	glEnd();

	// Between SC and SD
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(525.0f, 20.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(525.0f, 20.1f, 730.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(900.0f, 20.1f, 730.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(900.0f, 20.1f, 600.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Texture mapping for floor
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_floor);

	// Between SA and SB
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 60.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 60.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 60.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 60.1f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.0f, 60.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.0f, 60.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 60.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 60.1f, -20.0f);

	glEnd();

	// Between SB and SC
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 60.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 60.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 60.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 60.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.0f, 60.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.0f, 60.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 60.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 60.1f, 280.0f);

	glEnd();

	// Between SC and SD
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 60.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 60.1f, 730.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 60.1f, 730.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 60.1f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.0f, 60.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.0f, 60.1f, 730.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 60.1f, 730.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 60.1f, 600.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Texture mapping for pool between two buildings
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_water);

	// Between SA and SB
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(650.0f, 25.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(650.0f, 25.1f, 60.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(700.0f, 25.1f, 60.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(700.0f, 25.1f, -20.0f);

	glEnd();

	// Between SB and SC
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(775.0f, 25.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(775.0f, 25.1f, 360.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(725.0f, 25.1f, 360.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(725.0f, 25.1f, 280.0f);

	glEnd();

	// Between SC and SD
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(650.0f, 25.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(650.0f, 25.1f, 680.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(700.0f, 25.1f, 680.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(700.0f, 25.1f, 600.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	
	// Texture mapping for glass
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_window);

	// Between SA and SB
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, -10.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 30.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 30.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, -10.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, 70.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, 70.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.1f, 100.0f, 40.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.1f, 100.0f, 80.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.1f, 60.0f, 80.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.1f, 60.0f, 40.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(814.9f, 100.0f, 40.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(814.9f, 100.0f, 80.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(814.9f, 60.0f, 80.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(814.9f, 60.0f, 40.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, -10.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 30.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 30.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, -10.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, 70.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, 70.0f);

	glEnd();

	// Between SB and SC
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, 310.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 350.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 350.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, 310.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, 390.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, 390.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.1f, 100.0f, 340.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.1f, 100.0f, 380.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.1f, 60.0f, 380.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.1f, 60.0f, 340.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.1f, 100.0f, 410.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.1f, 100.0f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.1f, 60.0f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.1f, 60.0f, 410.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(814.9f, 100.0f, 360.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(814.9f, 100.0f, 400.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(814.9f, 60.0f, 400.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(814.9f, 60.0f, 360.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, 300.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 340.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 340.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, 300.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, 380.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, 380.0f);

	glEnd();

	// Between SC and SD
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, 615.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 655.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 655.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, 615.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(559.9f, 100.0f, 695.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(559.9f, 100.0f, 725.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(559.9f, 60.0f, 725.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(559.9f, 60.0f, 695.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.1f, 100.0f, 670.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.1f, 100.0f, 710.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.1f, 60.0f, 710.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.1f, 60.0f, 670.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(814.9f, 100.0f, 660.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(814.9f, 100.0f, 700.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(814.9f, 60.0f, 700.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(814.9f, 60.0f, 660.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 640.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 640.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(865.1f, 100.0f, 680.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(865.1f, 100.0f, 720.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.1f, 60.0f, 720.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.1f, 60.0f, 680.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Texture mapping for walls
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_wall);

	// Between SA and SB
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 102.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 102.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 100.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.0f, 102.1f, 100.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.0f, 102.1f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 30.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.0f, 102.1f, 30.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.0f, 102.1f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(765.0f, 102.1f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(765.0f, 102.1f, 50.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 50.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.0f, 30.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 20.0f, 30.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.0f, 20.0f, 30.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.0f, 102.0f, 30.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(650.1f, 102.0f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(650.1f, 102.0f, 30.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.1f, 20.0f, 30.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.1f, 20.0f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(609.9f, 60.0f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(609.9f, 60.0f, 30.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(609.9f, 20.0f, 30.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(609.9f, 20.0f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(765.0f, 102.0f, 50.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(765.0f, 20.0f, 50.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 20.0f, 50.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.0f, 50.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(764.9f, 102.0f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(764.9f, 102.0f, 50.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(764.9f, 20.0f, 50.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(764.9f, 20.0f, -20.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.1f, 60.0f, -20.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.1f, 60.0f, 50.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.1f, 20.0f, 50.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.1f, 20.0f, -20.0f);

	glEnd();

	// Between SB and SC
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 102.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 102.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 420.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.0f, 102.1f, 420.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.0f, 102.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 350.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(660.0f, 102.1f, 350.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(660.0f, 102.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(775.0f, 102.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(775.0f, 102.1f, 330.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 330.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.0f, 350.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 20.0f, 350.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(660.0f, 20.0f, 350.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(660.0f, 102.0f, 350.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(609.9f, 60.0f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(609.9f, 60.0f, 350.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(609.9f, 20.0f, 350.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(609.9f, 20.0f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(660.1f, 102.0f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(660.1f, 102.0f, 350.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(660.1f, 20.0f, 350.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(660.1f, 20.0f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(775.0f, 102.0f, 330.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(775.0f, 20.0f, 330.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 20.0f, 330.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.0f, 330.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(774.9f, 102.0f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(774.9f, 102.0f, 330.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(774.9f, 20.0f, 330.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(774.9f, 20.0f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(775.0f, 102.1f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(775.0f, 102.1f, 330.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 330.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, 280.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.1f, 60.0f, 280.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.1f, 60.0f, 330.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.1f, 20.0f, 330.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.1f, 20.0f, 280.0f);

	glEnd();

	// Between SC and SD
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(560.0f, 102.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(560.0f, 102.1f, 730.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 730.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 730.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(865.0f, 102.1f, 730.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(865.0f, 102.1f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 102.1f, 650.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.0f, 102.1f, 650.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.0f, 102.1f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(765.0f, 102.1f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(765.0f, 102.1f, 670.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 102.1f, 670.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.1f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(610.0f, 102.0f, 650.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(610.0f, 20.0f, 650.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.0f, 20.0f, 650.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.0f, 102.0f, 650.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(650.1f, 102.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(650.1f, 102.0f, 650.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(650.1f, 20.0f, 650.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(650.1f, 20.0f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(609.9f, 60.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(609.9f, 60.0f, 650.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(609.9f, 20.0f, 650.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(609.9f, 20.0f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(765.0f, 102.0f, 670.1f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(765.0f, 20.0f, 670.1f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.0f, 20.0f, 670.1f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.0f, 102.0f, 670.1f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(764.9f, 102.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(764.9f, 102.0f, 670.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(764.9f, 20.0f, 670.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(764.9f, 20.0f, 600.0f);

	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(815.1f, 60.0f, 600.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(815.1f, 60.0f, 670.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(815.1f, 20.0f, 670.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(815.1f, 20.0f, 600.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);


	// Blend function for glass
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor4f(0.0f, 0.0f, 1.0f, 0.2f);

	// Between SA and SB
	construct_cuboid(560.0f, 60.0f, -20.0f, 50.0f, 40.0f, 120.0f);
	build_cuboid();

	construct_cuboid(865.0f, 60.0f, -20.0f, -50.0f, 40.0f, 120.0f);
	build_cuboid();

	// Between SB and SC
	construct_cuboid(560.0f, 60.0f, 280.0f, 50.0f, 40.0f, 140.0f);
	build_cuboid();

	construct_cuboid(865.0f, 60.0f, 280.0f, -50.0f, 40.0f, 140.0f);
	build_cuboid();

	// Between SC and SD
	construct_cuboid(560.0f, 60.0f, 600.0f, 50.0f, 40.0f, 130.0f);
	build_cuboid();

	construct_cuboid(865.0f, 60.0f, 600.0f, -50.0f, 40.0f, 130.0f);
	build_cuboid();

	glDisable(GL_BLEND);


	// Walls of connection
	// Bottom
	glColor3f(1.0f, 153.0f / 255.0f, 51.0f / 255.0f);

	// Between SA and SB
	construct_cuboid(560.0f, 55.0f, -20.0f, 50.0f, 5.0f, 120.0f);
	build_cuboid();

	construct_cuboid(865.0f, 55.0f, -20.0f, -50.0f, 5.0f, 120.0f);
	build_cuboid();

	// Between SB and SC
	construct_cuboid(560.0f, 55.0f, 280.0f, 50.0f, 5.0f, 140.0f);
	build_cuboid();

	construct_cuboid(865.0f, 55.0f, 280.0f, -50.0f, 5.0f, 140.0f);
	build_cuboid();

	// Between SC and SD
	construct_cuboid(560.0f, 55.0f, 600.0f, 50.0f, 5.0f, 130.0f);
	build_cuboid();

	construct_cuboid(865.0f, 55.0f, 600.0f, -50.0f, 5.0f, 130.0f);
	build_cuboid();


	// Top
	glColor3f(1.0f, 1.0f, 1.0f);

	// Between SA and SB
	construct_cuboid(560.0f, 100.0f, -20.0f, 50.0f, 2.0f, 120.0f);
	build_cuboid();

	construct_cuboid(865.0f, 100.0f, -20.0f, -50.0f, 2.0f, 120.0f);
	build_cuboid();

	// Between SB and SC
	construct_cuboid(560.0f, 100.0f, 280.0f, 50.0f, 2.0f, 140.0f);
	build_cuboid();

	construct_cuboid(865.0f, 100.0f, 280.0f, -50.0f, 2.0f, 140.0f);
	build_cuboid();

	// Between SC and SD
	construct_cuboid(560.0f, 100.0f, 600.0f, 50.0f, 2.0f, 130.0f);
	build_cuboid();

	construct_cuboid(865.0f, 100.0f, 600.0f, -50.0f, 2.0f, 130.0f);
	build_cuboid();


	// Rooftop between two buildings
	glColor3f(1.0f, 1.0f, 1.0f);

	// Between SA and SB
	construct_cuboid(610.0f, 20.0f, -20.0f, 40.0f, 82.0f, 50.0f);
	build_cuboid();

	construct_cuboid(815.0f, 20.0f, -20.0f, -50.0f, 82.0f, 70.0f);
	build_cuboid();

	// Between SB and SC
	construct_cuboid(610.0f, 20.0f, 280.0f, 50.0f, 82.0f, 70.0f);
	build_cuboid();

	construct_cuboid(815.0f, 20.0f, 280.0f, -40.0f, 82.0f, 50.0f);
	build_cuboid();

	// Between SC and SD
	construct_cuboid(610.0f, 20.0f, 600.0f, 40.0f, 82.0f, 50.0f);
	build_cuboid();

	construct_cuboid(815.0f, 20.0f, 600.0f, -50.0f, 82.0f, 70.0f);
	build_cuboid();


	// Railings of the Rooftop
	glColor3f(1.0f, 1.0f, 1.0f);

	// Horizontal
	glBegin(GL_LINES);

	// Between SA and SB
	glVertex3f(560.0f, 120.0f, -20.0f);
	glVertex3f(560.0f, 120.0f, 100.0f);

	glVertex3f(610.0f, 120.0f, 30.0f);
	glVertex3f(610.0f, 120.0f, 100.0f);

	glVertex3f(650.0f, 120.0f, -20.0f);
	glVertex3f(650.0f, 120.0f, 30.0f);

	glVertex3f(610.0f, 120.0f, 30.0f);
	glVertex3f(650.0f, 120.0f, 30.0f);

	glVertex3f(765.0f, 120.0f, -10.0f);
	glVertex3f(765.0f, 120.0f, 50.0f);

	glVertex3f(765.0f, 120.0f, -10.0f);
	glVertex3f(765.0f, 102.0f, -10.0f);

	glVertex3f(765.0f, 120.0f, 50.0f);
	glVertex3f(815.0f, 120.0f, 50.0f);

	glVertex3f(815.0f, 120.0f, 50.0f);
	glVertex3f(815.0f, 120.0f, 100.0f);

	glVertex3f(865.0f, 120.0f, -20.0f);
	glVertex3f(865.0f, 120.0f, 100.0f);

	// Between SB and SC
	glVertex3f(560.0f, 120.0f, 280.0f);
	glVertex3f(560.0f, 120.0f, 420.0f);

	glVertex3f(660.0f, 120.0f, 290.0f);
	glVertex3f(660.0f, 120.0f, 350.0f);

	glVertex3f(660.0f, 120.0f, 290.0f);
	glVertex3f(660.0f, 102.0f, 290.0f);

	glVertex3f(660.0f, 120.0f, 350.0f);
	glVertex3f(610.0f, 120.0f, 350.0f);

	glVertex3f(610.0f, 120.0f, 350.0f);
	glVertex3f(610.0f, 120.0f, 420.0f);

	glVertex3f(775.0f, 120.0f, 280.0f);
	glVertex3f(775.0f, 120.0f, 330.0f);

	glVertex3f(775.0f, 120.0f, 330.0f);
	glVertex3f(815.0f, 120.0f, 330.0f);

	glVertex3f(865.0f, 120.0f, 280.0f);
	glVertex3f(865.0f, 120.0f, 420.0f);

	glVertex3f(815.0f, 120.0f, 330.0f);
	glVertex3f(815.0f, 120.0f, 420.0f);

	// Between SC and SD
	glVertex3f(560.0f, 120.0f, 600.0f);
	glVertex3f(560.0f, 120.0f, 730.0f);

	glVertex3f(610.0f, 120.0f, 650.0f);
	glVertex3f(610.0f, 120.0f, 730.0f);

	glVertex3f(650.0f, 120.0f, 600.0f);
	glVertex3f(650.0f, 120.0f, 650.0f);

	glVertex3f(610.0f, 120.0f, 650.0f);
	glVertex3f(650.0f, 120.0f, 650.0f);

	glVertex3f(765.0f, 120.0f, 610.0f);
	glVertex3f(765.0f, 120.0f, 670.0f);

	glVertex3f(765.0f, 120.0f, 610.0f);
	glVertex3f(765.0f, 102.0f, 610.0f);

	glVertex3f(765.0f, 120.0f, 670.0f);
	glVertex3f(815.0f, 120.0f, 670.0f);

	glVertex3f(815.0f, 120.0f, 670.0f);
	glVertex3f(815.0f, 120.0f, 730.0f);

	glVertex3f(865.0f, 120.0f, 600.0f);
	glVertex3f(865.0f, 120.0f, 730.0f);


	glEnd();

	// Vertical
	glBegin(GL_LINES);
	// Between SA and SB
	glVertex3f(610.0f, 102.0f, 30.0f);
	glVertex3f(610.0f, 120.0f, 30.0f);

	glVertex3f(650.0f, 102.0f, 30.0f);
	glVertex3f(650.0f, 120.0f, 30.0f);

	glVertex3f(560.0f, 102.0f, 30.0f);
	glVertex3f(560.0f, 120.0f, 30.0f);

	glVertex3f(765.0f, 102.0f, 50.0f);
	glVertex3f(765.0f, 120.0f, 50.0f);

	glVertex3f(815.0f, 102.0f, 50.0f);
	glVertex3f(815.0f, 120.0f, 50.0f);

	glVertex3f(865.0f, 102.0f, 50.0f);
	glVertex3f(865.0f, 120.0f, 50.0f);

	// Between SB and SC
	glVertex3f(560.0f, 102.0f, 350.0f);
	glVertex3f(560.0f, 120.0f, 350.0f);

	glVertex3f(610.0f, 102.0f, 350.0f);
	glVertex3f(610.0f, 120.0f, 350.0f);

	glVertex3f(660.0f, 102.0f, 350.0f);
	glVertex3f(660.0f, 120.0f, 350.0f);

	glVertex3f(775.0f, 102.0f, 330.0f);
	glVertex3f(775.0f, 120.0f, 330.0f);

	glVertex3f(815.0f, 102.0f, 330.0f);
	glVertex3f(815.0f, 120.0f, 330.0f);

	glVertex3f(865.0f, 102.0f, 330.0f);
	glVertex3f(865.0f, 120.0f, 330.0f);

	// Between SC and SD
	glVertex3f(610.0f, 102.0f, 650.0f);
	glVertex3f(610.0f, 120.0f, 650.0f);

	glVertex3f(650.0f, 102.0f, 650.0f);
	glVertex3f(650.0f, 120.0f, 650.0f);

	glVertex3f(560.0f, 102.0f, 650.0f);
	glVertex3f(560.0f, 120.0f, 650.0f);

	glVertex3f(765.0f, 102.0f, 670.0f);
	glVertex3f(765.0f, 120.0f, 670.0f);

	glVertex3f(815.0f, 102.0f, 670.0f);
	glVertex3f(815.0f, 120.0f, 670.0f);

	glVertex3f(865.0f, 102.0f, 670.0f);
	glVertex3f(865.0f, 120.0f, 670.0f);

	glEnd();


	// Pool between two buildings
	glColor3f(141.0f / 255.0f, 193.0f / 255.0f, 241.0f / 255.0f);

	// Between SA and SB
	construct_cuboid(650.0f, 20.0f, -20.0f, 50.0f, 5.0f, 80.0f);
	build_cuboid();

	// Between SB and SC
	construct_cuboid(775.0f, 20.0f, 280.0f, -50.0f, 5.0f, 80.0f);
	build_cuboid();

	// Between SC and SD
	construct_cuboid(650.0f, 20.0f, 600.0f, 50.0f, 5.0f, 80.0f);
	build_cuboid();


	// Steps beside the platform
	glColor3f(1.0f, 1.0f, 1.0f);

	// Between SA and SB
	GLfloat z = -20.0f;
	GLfloat y = 100.0f;
	
	for (y = 100.0f; y > 20.0f; y -= 10.0f)
	{
		construct_cuboid(765.0f, y, z, -15.0f, -y + 20.0f, 10.0f);
		build_cuboid();

		z += 10.0f;
	}
	//construct_cuboid(765.0f, 100.0f, -20.0f, -15.0f, -100.0f, 10.0f);
	//build_cuboid();

	// Between SB and SC
	z = 280.0f;
	y = 100.0f;

	for (y = 100.0f; y > 20.0f; y -= 10.0f)
	{
		construct_cuboid(660.0f, y, z, 15.0f, -y + 20.0f, 10.0f);
		build_cuboid();

		z += 10.0f;
	}

	// Between SC and SD
	z = 600.0f;
	y = 100.0f;

	for (y = 100.0f; y > 20.0f; y -= 10.0f)
	{
		construct_cuboid(765.0f, y, z, -15.0f, -y + 20.0f, 10.0f);
		build_cuboid();

		z += 10.0f;
	}

}


// Function to manage Science Buildings
void manage_science_buildings()
{
	// Draw Science Buildings -> use translate to copy and paste buildings
	// SA
	draw_science_building();

	// SB
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 300.0f);
	draw_science_building();
	glPopMatrix();

	// SC
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 620.0f);
	draw_science_building();
	glPopMatrix();

	// SD
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 910.0f);
	draw_science_building();
	glPopMatrix();

	// Connection between each building
	draw_connection();
}


// Function to define the display function
void define_openGL(void)
{
	// Set the width of line
	glLineWidth(1.0f);

	// Set the color of the background -> simulate sunny days
	glClearColor(204.0f / 255.0f, 229.0f / 255.0f, 255.0f / 255.0f, 0.4f);

	// enable or disable server-side GL capabilities
	glEnable(GL_DEPTH_TEST);

	// Set the viewport to be the entire window
	glViewport(0, 0, width, height);

	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Define the clipping volume
	// Create an orthogonal and parallel viewing volume
	// glOrtho(left, right, bottom, top, near, far)
	glOrtho(-width, width, -height, height, -width, width);

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Rotate
	glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotate_z, 0.0f, 0.0f, 1.0f);

	// Scale
	glScalef(scale_factor, scale_factor, scale_factor);

	// Translate
	glTranslatef(-225.0f, 0.0f, -470.0f);

	// Set the material
	set_material(diffuse_material);

	// Set smooth effect
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	// Draw ground
	draw_ground();

	// Draw pool
	draw_pool();

	// Draw Center Building
	draw_center_building();

	// Draw Science Buildings
	manage_science_buildings();

	// Draw the name of school
	draw_name();

	// Manage trees
	manage_tree();

	glutSwapBuffers();
}


// The main function of the program
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(150, 15);
	glutInitWindowSize(width, height);
	glutCreateWindow("The 3-dimensional scene created by openGL");

	myinit();

	// Enable the depth test
	glEnable(GL_DEPTH_TEST);

	// Enable texture
	glEnable(GL_TEXTURE_2D);

	// Load files used to make texture mapping need to stored in the same folder with .cpp file
	texture_road = texture_mapping("road.bmp");
	texture_grass = texture_mapping("grass.bmp");
	texture_water = texture_mapping("water.bmp");
	texture_floor = texture_mapping("floor.bmp");
	texture_red = texture_mapping("red.bmp");
	texture_window = texture_mapping("window.bmp");
	texture_wall = texture_mapping("wall.bmp");
	texture_roof_cb = texture_mapping("roof_cb.bmp");
	texture_soil = texture_mapping("soil.bmp");

	glutIdleFunc(when_in_mainloop);

	glutDisplayFunc(define_openGL);

	glutTimerFunc(time_interval, timer_boat, 1);
	glutTimerFunc(time_interval_ufo, timer_ufo, 1);

	// call the keyboard interaction defined in the above
	glutKeyboardFunc(keyboard_input);

	// call the special key interaction defined in the above
	glutSpecialFunc(special_key_input);

	// call the mouse interaction defined in the above
	glutMouseFunc(mouse_input);

	glutMainLoop();
}
