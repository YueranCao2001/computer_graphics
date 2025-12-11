# define FREEGLUT_STATIC

// There are only 0 to 127 ASCII characters in total, 128 characters in total
# define MAX_CHAR 128

// Define a PI to calculate the circle and ellipse
# define PI 3.1415926

# include <GL/freeglut.h>
# include <math.h>
# include <stdlib.h>
# include <stdio.h>
# include <windows.h>

// Declare incremental step
GLfloat step = 1;

// Declare refresh interval in ms
int time_interval = 16;

// Factor of transltion and scaling
float y_t = 0.0f;
float x_t = 0.0f;
float s = 1.0f;

// Determine the polygon is fill or not
bool flag = true;

// Force OpenGL to redraw the current window
void when_in_mainloop()
{
	glutPostRedisplay();
}

// Dynamic_balloons
void on_y_timer(int value)
{
	y_t += step;
	if (y_t >= 1100)
		y_t = 0;
	else if (y_t <= 0)
		y_t = 1099;

	// when_in_mainloop();
	glutTimerFunc(time_interval, on_y_timer, 1);
}

// Dynamic_clouds
void on_x_timer(int value)
{
	x_t += step;
	if (x_t >= 450)
		x_t = 0;
	else if (x_t <= 0)
		x_t = 449;

	// when_in_mainloop();
	glutTimerFunc(time_interval, on_x_timer, 1);
}

// Keyboard_interaction
void keyboard_input(unsigned char key, int x, int y)
{
	// q|Q: quit
	if (key == 'q' || key == 'Q')
		exit(0);

	// f|F: flag status
	else if (key == 'F' || key == 'f')
	{
		if (flag == true)
			flag = false;
		else
			flag = true;
	}

	// r|R: reset step (resume movement)
	else if (key == 'r' || key == 'R')
		step = 1;

	// o|O: change direction of movement (opposite direction)
	else if (key == 'o' || key == 'O')
		step = -step;

	// t|T: stop movement (Termination)
	else if (key == 't' || key == 'T')
		step = 0;

	// Change the size of trees and balls
	// b|B: bigger
	else if (key == 'b' || key == 'B')
		s += 0.01f;

	// s|S: smaller
	else if (key == 's' || key == 'S')
		s -= 0.01f;

	// c|C: callback parameter
	else if (key == 'c' || key == 'C')
		s = 1.0f;
}

// Mouse_interaction
void mouse_input(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && step >= -15)
		step -= 1;
	else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON && step <= 15)
		step += 1;
}

// Method to draw English or Chinese
void drawString(const char* str)
{
	int len;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// Count the number of characters
	// If it is a double byte character (such as Chinese character), two bytes are counted as one character
	// Otherwise, a byte counts as a character
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

// Method to select font
void selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

// Method to draw the background of the greeting card
void draw_background()
{
	glPointSize(50.0f);

	glBegin(GL_POLYGON);
	glColor3f(6.0f / 255.0f, 3.0f / 255.0f, 70.0f / 255.0f);
	glVertex2f(0.0f, 780.0f);
	glColor3f(0.0f, 0.0f, 67.0f / 255.0f);
	glVertex2f(0.0f, 0.0f);
	glColor3f(97.0f / 255.0f, 45.0f / 255.0f, 115.0f / 255.0f);
	glVertex2f(1100.0f, 0.0f);
	glColor3f(141.0f / 255.0f, 66.0f / 255.0f, 138.0f / 255.0f);
	glVertex2f(1100.0f, 780.0f);
	glEnd();
}

// Background of the word "anniversary"
void draw_anniversary_background() {
	glColor3f(3.0f / 255.0f, 101.0f / 255.0f, 126.0f / 255.0f);

	// Body
	glBegin(GL_POLYGON);
	glVertex2f(130.7f, 555.0f);
	glVertex2f(460.1f, 555.0f);
	glVertex2f(460.1f, 625.1f);
	glVertex2f(130.7f, 625.1f);
	glEnd();

	// Left
	glBegin(GL_POLYGON);
	glVertex2f(75.3f, 567.2f);
	glVertex2f(78.0f, 535.6f);
	glVertex2f(130.7f, 545.3f);
	glVertex2f(130.7f, 580.5f);
	glEnd();

	// Right
	glBegin(GL_POLYGON);
	glVertex2f(460.1f, 580.5f);
	glVertex2f(460.1f, 545.3f);
	glVertex2f(512.8f, 535.6f);
	glVertex2f(515.5f, 567.2f);
	glEnd();

	// Body_left
	glBegin(GL_TRIANGLES);
	glVertex2f(130.7f, 555.0f);
	glVertex2f(130.7f, 545.3f);
	glVertex2f(189.2f, 555.0f);
	glEnd();

	// Body_right
	glBegin(GL_TRIANGLES);
	glVertex2f(401.6f, 555.0f);
	glVertex2f(460.1f, 545.3f);
	glVertex2f(460.1f, 555.3f);
	glEnd();

	// Left_top
	glBegin(GL_TRIANGLES);
	glVertex2f(56.5f, 563.2f);
	glVertex2f(76.8f, 550.7f);
	glVertex2f(75.3f, 567.2f);
	glEnd();

	// Left_bottom
	glBegin(GL_TRIANGLES);
	glVertex2f(76.8f, 550.7f);
	glVertex2f(65.3f, 533.5f);
	glVertex2f(78.0f, 535.6f);
	glEnd();

	// Right_top
	glBegin(GL_TRIANGLES);
	glVertex2f(515.5f, 567.7f);
	glVertex2f(514.0f, 550.7f);
	glVertex2f(534.3f, 563.2f);
	glEnd();

	// Right_bottom
	glBegin(GL_TRIANGLES);
	glVertex2f(514.0f, 550.7f);
	glVertex2f(512.8f, 535.6f);
	glVertex2f(526.7f, 533.5f);
	glEnd();

	/* Border stroke */
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	// Body
	glVertex2f(130.7f, 555.0f);
	glVertex2f(460.1f, 555.0f);

	glVertex2f(460.1f, 555.0f);
	glVertex2f(460.1f, 625.1f);

	glVertex2f(460.1f, 625.1f);
	glVertex2f(130.7f, 625.1f);

	glVertex2f(130.7f, 625.1f);
	glVertex2f(130.7f, 555.0f);

	// Left
	glVertex2f(130.7f, 580.5f);
	glVertex2f(56.5f, 563.2f);

	glVertex2f(56.5f, 563.2f);
	glVertex2f(76.8f, 550.7f);

	glVertex2f(76.8f, 550.7f);
	glVertex2f(65.3f, 533.5f);

	glVertex2f(65.3f, 533.5f);
	glVertex2f(189.2f, 555.0f);

	// Right
	glVertex2f(460.1f, 580.5f);
	glVertex2f(534.3f, 563.2f);

	glVertex2f(534.3f, 563.2f);
	glVertex2f(514.0f, 550.7f);

	glVertex2f(514.0f, 550.7f);
	glVertex2f(526.7f, 533.5f);

	glVertex2f(526.7f, 533.5f);
	glVertex2f(401.6f, 555.0f);

	glEnd();

	// Restore lineweight
	glLineWidth(1.0f);
}

// Method to draw balls above the anniversary
void draw_anniversary_ball()
{
	const int n = 200;
	const GLfloat r = 7.0f;
	
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r * cos(2 * PI * i / n), r * sin(2 * PI * i / n));
	}
	glEnd();
}

// Method to arrange balls to their proper position
void draw_anniversary_balls()
{
	/* Red balls */
	glColor3f(230.0f / 255.0f, 0.0f, 0.0f);

	// Red_1
	glPushMatrix();
	glTranslatef(110.8f, 670.4f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_2
	glPushMatrix();
	glTranslatef(170.5f, 715.8f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_3
	glPushMatrix();
	glTranslatef(75.3f, 705.2f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_4
	glPushMatrix();
	glTranslatef(92.1f, 588.5f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_5
	glPushMatrix();
	glTranslatef(480.0f, 660.2f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_6
	glPushMatrix();
	glTranslatef(420.9f, 710.8f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_7
	glPushMatrix();
	glTranslatef(520.8f, 690.0f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Red_8
	glPushMatrix();
	glTranslatef(487.5f, 586.7f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow balls
	glColor3f(1.0f, 1.0f, 0.0f);

	// Yellow_1
	glPushMatrix();
	glTranslatef(130.3f, 740.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_2
	glPushMatrix();
	glTranslatef(54.9f, 600.0f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_3
	glPushMatrix();
	glTranslatef(150.2f, 660.7f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_4
	glPushMatrix();
	glTranslatef(80.2f, 675.8f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_5
	glPushMatrix();
	glTranslatef(523.9f, 600.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_6
	glPushMatrix();
	glTranslatef(455.3f, 740.8f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_7
	glPushMatrix();
	glTranslatef(435.1f, 675.1f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Yellow_8
	glPushMatrix();
	glTranslatef(507.0f, 670.6f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	/* Cyan */
	glColor3f(3.0f / 255.0f, 101.0f / 255.0f, 126.0f / 255.0f);

	// Cyan_1
	glPushMatrix();
	glTranslatef(80.0f, 630.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_2
	glPushMatrix();
	glTranslatef(105.5f, 708.9f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_3
	glPushMatrix();
	glTranslatef(140.7f, 715.7f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_4
	glPushMatrix();
	glTranslatef(113.4f, 630.5f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_5
	glPushMatrix();
	glTranslatef(490.2f, 715.2f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_6
	glPushMatrix();
	glTranslatef(500.8f, 628.9f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_7
	glPushMatrix();
	glTranslatef(437.1f, 725.8f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Cyan_8
	glPushMatrix();
	glTranslatef(440.0f, 700.0f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	/* Blue */
	glColor3f(51.0f / 255.0f, 153.0f / 255.0f, 1.0f);

	// Blue_1
	glPushMatrix();
	glTranslatef(175.3f, 745.0f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_2
	glPushMatrix();
	glTranslatef(135.1f, 680.5f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_3
	glPushMatrix();
	glTranslatef(50.5f, 650.0f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_4
	glPushMatrix();
	glTranslatef(185.8f, 685.8f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_5
	glPushMatrix();
	glTranslatef(420.2f, 750.6f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_6
	glPushMatrix();
	glTranslatef(465.2f, 690.3f, 0.0f);
	glScalef(0.8f * s, 0.8f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_7
	glPushMatrix();
	glTranslatef(535.1f, 640.8f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();

	// Blue_8
	glPushMatrix();
	glTranslatef(475.7f, 625.4f, 0.0f);
	glScalef(0.4f * s, 0.4f * s, 0.0f);
	draw_anniversary_ball();
	glPopMatrix();
}

// Draw "2006 - 2021"
void draw_word_year(const char* num, GLfloat y)
{
	selectFont(64, ANSI_CHARSET, "Bauhaus 93");
	glRasterPos2f(600.3f, y);
	drawString(num);
}

// Method to create and arrange words to their proper position
void draw_word()
{
	/*The front side of card*/
	// Full name of "XJTLU"
	glColor3f(1.0f, 1.0f, 1.0f);
	selectFont(22, ANSI_CHARSET, "Helvetica");
	glRasterPos2f(151.5f, 529.0f);
	drawString(" Xi'an Jiaotong-Liverpool University");

	// "XJTLU"
	selectFont(64, ANSI_CHARSET, "Helvetica");
	glRasterPos2f(667.9f, 40.3f);
	drawString("XJTLU");

	// Chinese Name of "XJTLU"
	selectFont(45, GB2312_CHARSET, "华文行楷");
	glRasterPos2f(145.5f, 487.9f);
	drawString("西交利物浦大学");

	// Draw "2006 - 2021"
	draw_word_year("2", 735.8f);
	draw_word_year("0", 685.8f);
	draw_word_year("0", 635.8f);
	draw_word_year("6", 585.8f);
	draw_word_year("~", 525.8f);
	draw_word_year("2", 500.8f);
	draw_word_year("0", 450.8f);
	draw_word_year("2", 400.8f);
	draw_word_year("1", 350.8f);

	// draw "15"
	glColor3f(249.0f / 255.0f, 180.f / 255.0f, 41.0f / 255.0f);
	selectFont(200, ANSI_CHARSET, "MV Boli");
	glRasterPos2f(220.0f, 650.0f);
	drawString("15");

	// draw "anniversary"
	draw_anniversary_background();
	draw_anniversary_balls();
	glColor3f(1.0f, 1.0f, 1.0f);
	selectFont(78, ANSI_CHARSET, "Harlow Solid Italic");
	glRasterPos2f(135.0f, 580.5f);
	drawString("Anniversary");
}

// Created by two ellipses and two circles(Position: on the road of Center Building)
void draw_tree_1()
{
	const GLfloat a1 = 18.6f;
	const GLfloat b = 8.86f;
	const GLfloat a2 = 20.3f;
	const GLfloat r1 = 12.0f;
	const GLfloat r2 = 11.0f;
	const int n = 200;

	// Trunk
	glColor3f(102.0f / 255.0f, 51.0f / 255.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(20.3f, 0.0f);
	glVertex2f(20.3f, 75.35f);
	glEnd();

	/* Leaves */
	// Top_circle
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	glPushMatrix();
	glTranslatef(20.3f, 62.95f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r1 * cos(2 * PI * i / n), r1 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	// Second_ellipse
	glPushMatrix();
	glTranslatef(20.3f, 51.4f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(a1 * cos(2 * PI * i / n), b * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	// Third_ellipse
	glPushMatrix();
	glTranslatef(20.3f, 36.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(a2 * cos(2 * PI * i / n), b * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	// Bottom_circle
	glPushMatrix();
	glTranslatef(20.3f, 29.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r2 * cos(2 * PI * i / n), r2 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();
}

// Created by one ellipse(Position: beside Center Building)
void draw_tree_2()
{
	const GLfloat a = 9.0f;
	const GLfloat b = 27.5f;
	const int n = 200;
	// Trunk
	glColor3f(102.0f / 255.0f, 51.0f / 255.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(10.0f, 0.0f);
	glVertex2f(10.0f, 63.8f);
	glEnd();

	// Leaves
	glColor3f(0.0f, 153.0f / 255.0f, 0.0f);
	glPushMatrix();
	glTranslatef(10.0f, 39.8f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(a * cos(2 * PI * i / n), b * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();
}

// DDA algorithm to draw center building
void dda_algorithm(GLint x1, GLint y1, GLint x2, GLint y2)
{
	GLint k = 0;
	if (abs(x2 - x1) > abs(y2 - y1))
		k = abs(x2 - x1);
	else
		k = abs(y2 - y1);

	GLfloat dx = (GLfloat)(x2 - x1) / k;
	GLfloat dy = (GLfloat)(y2 - y1) / k;
	GLfloat x = x1;
	GLfloat y = y1;

	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);

	for (GLint i = 0; i < k; ++i)
	{
		glBegin(GL_POINTS);
		// Rounding
		glVertex2i((GLint)round(x) , (GLint)round(y));
		glEnd();
		x += dx;
		y += dy;
	}
}

// Method to draw and arrange objects of Center Building
void draw_center_building()
{
	// Draw road
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f(673.75f, 380.1f);
	glVertex2f(1018.6f, 501.6f);

	glVertex2f(878.5f, 381.0f);
	glVertex2f(1069.1f, 501.6f);
	glEnd();

	/* Draw trees on the road */
	// Left_1
	glPushMatrix();
	glTranslatef(725.9f, 412.97f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0);
	draw_tree_1();
	glPopMatrix();

	// Left_2
	glPushMatrix();
	glTranslatef(775.89f, 430.5f,0.0f);
	glScalef(0.75f * s, 0.75f * s, 0);
	draw_tree_1();
	glPopMatrix();

	// Left_3
	glPushMatrix();
	glTranslatef(815.5f, 442.0f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0);
	draw_tree_1();
	glPopMatrix();

	// Left_4
	glPushMatrix();
	glTranslatef(850.65f, 453.0f, 0.0f);
	glScalef(0.45f * s, 0.45f * s, 0.0f);
	draw_tree_1();
	glPopMatrix();

	// Right_1
	glPushMatrix();
	glTranslatef(920.6f, 389.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_1();
	glPopMatrix();

	// Right_2
	glPushMatrix();
	glTranslatef(965.6f, 415.8f, 0.0f);
	glScalef(0.75f * s, 0.75 * s, 0.0f);
	draw_tree_1();
	glPopMatrix();

	// Right_3
	glPushMatrix();
	glTranslatef(1000.7f, 435.3f, 0.0f);
	glScalef(0.6f * s, 0.6f * s, 0.0f);
	draw_tree_1();
	glPopMatrix();

	// Right_4
	glPushMatrix();
    glTranslatef(1030.4f, 450.4f, 0.0f);
	glScalef(0.45f * s, 0.45f * s, 0.0f);
	draw_tree_1();
	glPopMatrix();

	// Draw center building: DDA algorithm part
	glPointSize(0.1f);
	dda_algorithm(770, 512, 886, 516);
	dda_algorithm(726, 516, 886, 522);
	dda_algorithm(726, 522, 886, 528);
	dda_algorithm(726, 528, 886, 534);
	dda_algorithm(726, 534, 886, 540);
	dda_algorithm(734, 540, 886, 546);
	dda_algorithm(734, 546, 886, 552);
	dda_algorithm(734, 552, 886, 558);
	dda_algorithm(734, 558, 886, 564);
	dda_algorithm(734, 564, 886, 569);
	dda_algorithm(734, 568, 886, 575);
	dda_algorithm(734, 573, 749, 574);
	dda_algorithm(734, 577, 749, 578);
	dda_algorithm(734, 581, 749, 582);
	dda_algorithm(734, 585, 749, 586);
	dda_algorithm(734, 589, 749, 590);
	dda_algorithm(766, 574, 886, 580);
	dda_algorithm(772, 578, 886, 586);
	dda_algorithm(780, 584, 886, 592);
	dda_algorithm(790, 590, 886, 598);
	dda_algorithm(810, 596, 886, 604);
	dda_algorithm(726, 592, 766, 598);
	dda_algorithm(726, 598, 783, 605);
	dda_algorithm(726, 604, 802, 612);
	dda_algorithm(726, 609, 822, 620);
	dda_algorithm(726, 615, 840, 628);
	dda_algorithm(726, 622, 866, 639);
	dda_algorithm(726, 628, 886, 650);
	dda_algorithm(726, 633, 886, 658);
	dda_algorithm(726, 638, 886, 668);
	dda_algorithm(726, 642, 886, 676);
	dda_algorithm(726, 648, 771, 659);
	dda_algorithm(726, 653, 769, 665);
	dda_algorithm(740, 661, 767, 670);
	dda_algorithm(785, 660, 886, 686);
	dda_algorithm(787, 668, 886, 696);
	dda_algorithm(788, 676, 886, 706);
	dda_algorithm(789, 684, 886, 716);
	dda_algorithm(815, 701, 886, 726);
	dda_algorithm(886, 726, 965, 682);
	dda_algorithm(886, 716, 965, 672);
	dda_algorithm(886, 706, 965, 662);
	dda_algorithm(886, 696, 965, 652);
	dda_algorithm(886, 686, 965, 642);
	dda_algorithm(886, 676, 965, 632);
	dda_algorithm(886, 668, 965, 624);
	dda_algorithm(886, 658, 952, 624);
	dda_algorithm(886, 650, 920, 636);
	dda_algorithm(1050, 635, 990, 670);
	dda_algorithm(1050, 629, 990, 665);
	dda_algorithm(1050, 623, 992, 657);
	dda_algorithm(1050, 617, 993, 650);
	dda_algorithm(1050, 611, 995, 642);
	dda_algorithm(1050, 605, 997, 634);
	dda_algorithm(1050, 599, 999, 626);
	dda_algorithm(1050, 593, 1001, 618);
	dda_algorithm(1050, 587, 1003, 610);
	dda_algorithm(1050, 581, 1006, 602);
	dda_algorithm(1050, 575, 1009, 594);
	dda_algorithm(1012, 586, 1040, 574);
	dda_algorithm(1011, 580, 1030, 574);
	dda_algorithm(886, 516, 1050, 513);
	dda_algorithm(886, 522, 1050, 516);
	dda_algorithm(886, 528, 1050, 522);
	dda_algorithm(886, 534, 1050, 528);
	dda_algorithm(886, 540, 1043, 533);
	dda_algorithm(886, 546, 1034, 538);
	dda_algorithm(886, 552, 1023, 543);
	dda_algorithm(886, 558, 1011, 548);
	dda_algorithm(886, 564, 1001, 553);
	dda_algorithm(886, 569, 995, 558);
	dda_algorithm(886, 575, 986, 563);
	dda_algorithm(886, 580, 976, 568);
	dda_algorithm(886, 586, 965, 573);
	dda_algorithm(886, 592, 954, 578);
	dda_algorithm(886, 598, 940, 586);
	dda_algorithm(886, 604, 920, 595);
	glPointSize(1.0f);

	/* Draw the foundation of the building */
	// Left part
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(673.75f, 501.6f);
	glVertex2f(673.75f, 511.9f);
	glVertex2f(886.875f, 511.9f);
	glVertex2f(886.875f, 501.6f);
	glEnd();

	// Right part << translation
	glPushMatrix();
	glTranslatef(213.125f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(673.75f, 501.6f);
	glVertex2f(673.75f, 511.9f);
	glVertex2f(886.875f, 511.9f);
	glVertex2f(886.875f, 501.6f);
	glEnd();
	glPopMatrix();

	// The body part of center building: some parts use GL_POLYGON may cause more error
	glBegin(GL_LINES);
	glVertex2f(886.875f, 734.7f);
	glVertex2f(886.875f, 511.9f);

	glVertex2f(886.875f, 734.7f);
	glVertex2f(786.4f, 688.8f);

	glVertex2f(786.4f, 688.8f);
	glVertex2f(783.9f, 659.8f);

	glVertex2f(773.7f, 655.8f);
	glVertex2f(783.9f, 659.8f);

	glVertex2f(766.8f, 676.2f);
	glVertex2f(773.7f, 655.8f);

	glVertex2f(766.8f, 676.2f);
	glVertex2f(774.8f, 676.2f);

	glVertex2f(774.8f, 676.2f);
	glVertex2f(778.0f, 657.8f);

	glVertex2f(774.8f, 676.2f);
	glVertex2f(784.5f, 681.8f);

	glVertex2f(766.8f, 676.2f);
	glVertex2f(726.0f, 655.0f);

	glVertex2f(726.0f, 655.0f);
	glVertex2f(726.0f, 589.3f);

	glVertex2f(726.0f, 589.3f);
	glVertex2f(734.0f, 590.3f);

	glVertex2f(734.0f, 538.8f);
	glVertex2f(734.0f, 590.3f);

	glVertex2f(734.0f, 538.8f);
	glVertex2f(726.0f, 538.8f);

	glVertex2f(726.0f, 538.8f);
	glVertex2f(726.0f, 511.9f);

	glVertex2f(794.3f, 592.4f);
	glVertex2f(762.4f, 574.3f);

	glVertex2f(762.4f, 574.3f);
	glVertex2f(749.1f, 571.6f);

	glVertex2f(749.1f, 571.6f);
	glVertex2f(749.1f, 592.0f);

	glVertex2f(749.1f, 592.0f);
	glVertex2f(794.3f, 607.1f);

	glVertex2f(831.5f, 622.1f);
	glVertex2f(794.3f, 607.1f);

	glVertex2f(831.5f, 622.1f);
	glVertex2f(886.875f, 646.1f);

	glVertex2f(794.3f, 592.4f);
	glVertex2f(886.875f, 611.5f);

	glVertex2f(965.1f, 679.4f);
	glVertex2f(980.8f, 670.8f);

	glVertex2f(1043.0f, 573.6f);
	glVertex2f(1043.0f, 533.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(965.1f, 690.2f);
	glVertex2f(965.1f, 620.3f);
	glVertex2f(886.875f, 646.1f);
	glVertex2f(886.875f, 734.7f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(987.1f, 677.4f);
	glVertex2f(1050.3f, 641.0f);
	glVertex2f(1050.3f, 573.6f);
	glVertex2f(1012.9f, 573.6f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(1050.3f, 530.0f);
	glVertex2f(1050.3f, 511.9f);
	glVertex2f(886.875f, 511.9f);
	glVertex2f(886.875f, 611.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(1005.4f, 573.6f);
	glVertex2f(980.8f, 670.8f);
	glVertex2f(987.1f, 677.4f);
	glVertex2f(1012.9f, 573.6f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(995.7f, 518.9f);
	glVertex2f(995.7f, 511.9f);
	glVertex2f(1075.3f, 511.9f);
	glVertex2f(1075.3f, 518.9f);
	glEnd();

	/* Draw trees beside center building */
	// Left_1
	glPushMatrix();
	glTranslatef(665.6f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Left_2
	glPushMatrix();
	glTranslatef(682.6f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Left_3
	glPushMatrix();
	glTranslatef(698.6f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Left_4
	glPushMatrix();
	glTranslatef(715.6f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Right_1
	glPushMatrix();
	glTranslatef(1048.3f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Right_2
	glPushMatrix();
	glTranslatef(1065.3f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();

	// Right_3
	glPushMatrix();
	glTranslatef(1082.0f, 511.9f, 0.0f);
	glScalef(1.0f * s, 1.0f * s, 0.0f);
	draw_tree_2();
	glPopMatrix();
}

// Method to draw International Research Center
void draw_international_research_center()
{
	// Body part
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 414.7f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(264.1f, 0.0f);
	glVertex2f(264.1f, 423.6f);
	glVertex2f(56.7f, 452.8f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(264.1f, 0.0f);
	glVertex2f(294.3f, 0.0f);
	glVertex2f(294.3f, 375.7f);
	glVertex2f(264.1f, 375.7f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(294.3f, 88.5f);
	glVertex2f(294.3f, 0.0f);
	glVertex2f(347.5f, 0.0f);
	glVertex2f(347.5f, 94.7f);
	glEnd();

	/* Detail part */
	// Structure
	glLineWidth(10.0f);
	glColor3f(6.0f / 255.0f, 3.0f / 255.0f, 70.0f / 255.0f);
	glBegin(GL_LINES);
	glVertex2f(56.7f, 392.7f);
	glVertex2f(255.1f, 380.7f);

	glVertex2f(250.1f, 360.7f);
	glVertex2f(250.1f, 100.7f);

	glVertex2f(61.7f, 360.7f);
	glVertex2f(61.7f, 290.7f);

	glVertex2f(61.7f, 270.7f);
	glVertex2f(61.7f, 200.7f);

	glVertex2f(61.7f, 180.7f);
	glVertex2f(61.7f, 110.7f);

	glVertex2f(277.1f, 358.7f);
	glVertex2f(277.1f, 280.7f);

	glVertex2f(277.1f, 260.7f);
	glVertex2f(277.1f, 190.7f);

	glVertex2f(277.1f, 170.7f);
	glVertex2f(277.1f, 100.7f);

	glVertex2f(5.0f, 355.7f);
	glVertex2f(5.0f, 285.7f);

	glVertex2f(5.0f, 265.7f);
	glVertex2f(5.0f, 195.7f);

	glVertex2f(5.0f, 175.7f);
	glVertex2f(5.0f, 105.7f);
	glEnd();

	// Structure_shadow
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	glBegin(GL_LINES);
	glVertex2f(-5.0f, 406.7f);
	glVertex2f(56.7f, 447.8f);

	glVertex2f(55.7f, 448.8f);
	glVertex2f(263.5f, 419.6f);

	glVertex2f(293.3f, 370.7f);
	glVertex2f(264.1f, 370.7f);

	glVertex2f(61.7f, 290.7f);
	glVertex2f(61.7f, 280.7f);

	glVertex2f(61.7f, 200.7f);
	glVertex2f(61.7f, 190.7f);

	glVertex2f(61.7f, 110.7f);
	glVertex2f(61.7f, 100.7f);

	glVertex2f(277.1f, 280.7f);
	glVertex2f(277.1f, 270.7f);

	glVertex2f(277.1f, 190.7f);
	glVertex2f(277.1f, 180.7f);

	glVertex2f(277.1f, 100.7f);
	glVertex2f(277.1f, 90.7f);

	glVertex2f(5.0f, 285.7f);
	glVertex2f(5.0f, 275.7f);

	glVertex2f(5.0f, 195.7f);
	glVertex2f(5.0f, 185.7f);

	glVertex2f(5.0f, 105.7f);
	glVertex2f(5.0f, 95.7f);

	glVertex2f(56.7f, 383.7f);
	glVertex2f(255.1f, 371.7f);

	glVertex2f(250.1f, 100.7f);
	glVertex2f(250.1f, 90.7f);
	glEnd();

	// Structure
	glLineWidth(12.0f);
	glColor3f(6.0f / 255.0f, 3.0f / 255.0f, 70.0f / 255.0f);
	glBegin(GL_LINES);
	glVertex2f(164.7f, 340.7f);
	glVertex2f(205.7f, 340.7f);

	glVertex2f(170.7f, 340.7f);
	glVertex2f(170.7f, 280.7f);

	glVertex2f(164.7f, 250.7f);
	glVertex2f(205.7f, 250.7f);

	glVertex2f(170.7f, 250.7f);
	glVertex2f(170.7f, 190.7f);

	glVertex2f(164.7f, 160.7f);
	glVertex2f(205.7f, 160.7f);

	glVertex2f(170.7f, 160.7f);
	glVertex2f(170.7f, 95.7f);	
	glEnd();

	// Structure_shadow
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	glBegin(GL_LINES);
	glVertex2f(170.7f, 280.7f);
	glVertex2f(170.7f, 270.7f);

	glVertex2f(170.7f, 190.7f);
	glVertex2f(170.7f, 180.7f);

	glVertex2f(170.7f, 95.7f);
	glVertex2f(170.7f, 85.7f);
	glEnd();

	glLineWidth(6.0f);
	glBegin(GL_LINES);
	glVertex2f(205.7f, 330.7f);
	glVertex2f(177.0f, 330.7f);

	glVertex2f(205.7f, 240.7f);
	glVertex2f(177.0f, 240.7f);

	glVertex2f(205.7f, 151.0f);
	glVertex2f(177.0f, 151.0f);
	glEnd();

	// Structure_shadow
	glColor3f(192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f);
	glLineWidth(4.0f);
	glBegin(GL_POLYGON);
	glVertex2f(300.3f, 102.5f);
	glVertex2f(300.3f, 94.5f);
	glVertex2f(347.5f, 100.7f);
	glVertex2f(347.5f, 108.7f);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(295.3f, 88.5f);
	glVertex2f(295.3f, 78.5f);

	glVertex2f(295.3f, 80.5f);
	glVertex2f(346.8f, 86.5f);
	glEnd();

	glPushMatrix();
	glTranslatef(0.0f, -10.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 87.2f);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(0.0f, 69.3f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(170.7f, 95.7f);
	glVertex2f(250.1f, 86.7f);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(250.1f, 86.7f);
	glVertex2f(294.3f, 90.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 39.7f);
	glVertex2f(20.3f, 34.4f);
	glVertex2f(20.3f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(20.3f, 0.0f);
	glVertex2f(20.3f, 34.4f);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(39.8f, 0.0f);

	glBegin(GL_LINES);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(150.7f, 42.4f);

	glVertex2f(150.7f, 42.4f);
	glVertex2f(281.9f, 15.8f);

	glVertex2f(118.7f, 10.9f);
	glVertex2f(346.8f, 18.4f);

	glVertex2f(20.3f, 34.4f);
	glVertex2f(39.8f, 34.4f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(89.5f, 10.9f);
	glVertex2f(89.5f, 0.0f);
	glVertex2f(112.5f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(112.5f, 0.0f);
	glVertex2f(89.5f, 10.9f);
	glVertex2f(118.7f, 10.9f);
	glVertex2f(153.3f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(112.5f, 20.0f);
	glVertex2f(112.5f, 10.9f);
	glVertex2f(150.7f, 10.9f);
	glVertex2f(138.2f, 20.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(150.7f, 10.9f);
	glVertex2f(150.7f, 42.4f);
	glVertex2f(169.3f, 32.6f);
	glVertex2f(169.3f, 10.9f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(169.3f, 10.9f);
	glVertex2f(169.3f, 31.6f);
	glVertex2f(213.6f, 29.6f);
	glVertex2f(213.6f, 13.9f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(150.7f, 42.4f);
	glVertex2f(169.3f, 32.6f);
	glVertex2f(213.6f, 29.6f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(39.8f, 0.0f);
	glVertex2f(45.8f, 0.0f);
	glVertex2f(45.8f, 34.4f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 87.2f);
	glVertex2f(0.0f, 69.3f);
	glVertex2f(10.0f, 65.3f);
	glVertex2f(10.0f, 84.2f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(36.9f, 67.8f);
	glVertex2f(36.9f, 85.7f);
	glEnd();
	glPopMatrix();

	// Structure
	glColor3f(6.0f / 255.0f, 3.0f / 255.0f, 70.0f / 255.0f);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 87.2f);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(0.0f, 69.3f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(170.7f, 95.7f);
	glVertex2f(250.1f, 86.7f);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(250.1f, 86.7f);
	glVertex2f(294.3f, 90.5f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 39.7f);
	glVertex2f(20.3f, 34.4f);
	glVertex2f(20.3f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(20.3f, 0.0f);
	glVertex2f(20.3f, 34.4f);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(39.8f, 0.0f);

	glBegin(GL_LINES);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(150.7f, 42.4f);

	glVertex2f(150.7f, 42.4f);
	glVertex2f(281.9f, 15.8f);

	glVertex2f(118.7f, 10.9f);
	glVertex2f(346.8f, 18.4f);

	glVertex2f(20.3f, 34.4f);
	glVertex2f(39.8f, 34.4f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(89.5f, 10.9f);
	glVertex2f(89.5f, 0.0f);
	glVertex2f(112.5f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(112.5f, 0.0f);
	glVertex2f(89.5f, 10.9f);
	glVertex2f(118.7f, 10.9f);
	glVertex2f(153.3f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(112.5f, 20.0f);
	glVertex2f(112.5f, 10.9f);
	glVertex2f(150.7f, 10.9f);
	glVertex2f(138.2f, 20.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(150.7f, 10.9f);
	glVertex2f(150.7f, 42.4f);
	glVertex2f(169.3f, 32.6f);
	glVertex2f(169.3f, 10.9f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(169.3f, 10.9f);
	glVertex2f(169.3f, 31.6f);
	glVertex2f(213.6f, 29.6f);
	glVertex2f(213.6f, 13.9f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2f(150.7f, 42.4f);
	glVertex2f(169.3f, 32.6f);
	glVertex2f(213.6f, 29.6f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(39.8f, 34.4f);
	glVertex2f(39.8f, 0.0f);
	glVertex2f(45.8f, 0.0f);
	glVertex2f(45.8f, 34.4f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 87.2f);
	glVertex2f(0.0f, 69.3f);
	glVertex2f(10.0f, 65.3f);
	glVertex2f(10.0f, 84.2f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(31.9f, 82.7f);
	glVertex2f(31.9f, 64.8f);
	glVertex2f(36.9f, 67.8f);
	glVertex2f(36.9f, 85.7f);
	glEnd();

	// Restore line width
	glLineWidth(1.0f);
}

// Method to draw rainbow in the bottom right corner
void draw_rainbow()
{
	glLineWidth(30.0f);
	/* Purple */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(204.0f / 255.0f, 0.0f, 204.0f / 255.0f);
	// Inner radius, out radius, number of sides for each radial section, number of radial divisions
	glutSolidTorus(140.2, 0.0, 90, 90);
	glPopMatrix();

	/* Blue */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(0.0f, 51.0f / 255.0f, 204.0f / 255.0f);
	glutSolidTorus(170.2, 0.0, 90, 90);
	glPopMatrix();

	/* Cyan */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(0.0f, 204.0f / 255.0f, 204.0f / 255.0f);
	glutSolidTorus(200.2, 0.0, 90, 90);
	glPopMatrix();

	/* Green */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(51.0f / 255.0f, 1.0f, 51.0f / 255.0f);
	glutSolidTorus(230.2, 0.0, 90, 90);
	glPopMatrix();

	/* Yellow */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(1.0f, 1.0f, 0.0f);
	glutSolidTorus(260.2, 0.0, 90, 90);
	glPopMatrix();

	/* Orange */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(1.0f, 128.0f / 255.0f, 0.0f);
	glutSolidTorus(290.2, 0.0, 90, 90);
	glPopMatrix();

	/* Red */
	glPushMatrix();
	glTranslated(750.0, 0.0, 0.0);
	glColor3f(230.0f / 255.0f, 0.0f, 0.0f);
	glutSolidTorus(320.2, 0.0, 90, 90);
	glPopMatrix();

	glLineWidth(1.0f);
}

// Method to draw balloons
void draw_balloon(GLfloat red, GLfloat green, GLfloat blue)
{
	const GLfloat a = 40.0f;
	const GLfloat b = 70.0f;
	int n = 200;

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 200.0f);
	glEnd();

	glColor3f(red, green, blue);
	glPushMatrix();
	glTranslatef(0.0f, 200.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(a * cos(2 * PI * i / n), b * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();
}

// Method to draw clouds
void draw_cloud()
{
	const GLfloat a = 30.0f;
	const GLfloat b = 10.0f;
	const GLfloat r1 = 7.0f;
	const GLfloat r2 = 10.0f;
	const GLfloat r3 = 15.0f;
	const GLfloat r4 = 8.0f;
	int n = 200;

	glColor3f(224.0f / 255.0f, 224.0f / 255.0f, 224.0f / 255.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(a * cos(2 * PI * i / n), b * sin(2 * PI * i / n));
	}
	glEnd();

	glPushMatrix();
	glTranslatef(-25.0f, 4.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r1 * cos(2 * PI * i / n), r1 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-15.0f, 10.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r2 * cos(2 * PI * i / n), r2 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.0f, 15.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r3 * cos(2 * PI * i / n), r3 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(25.0f, 5.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(r4 * cos(2 * PI * i / n), r4 * sin(2 * PI * i / n));
	}
	glEnd();
	glPopMatrix();
}

// Method to arrange balloons and clouds to their proper position
void dynamic_scene()
{
	// ballon
	glPushMatrix();
	glTranslatef(560.3f, -300.0f, 0.0f);
	glTranslatef(0.0f, y_t, 0.0f);
	draw_balloon(102.0f / 255.0f, 178.0f / 255.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(510.3f, -300.0f, 0.0f);
	glTranslatef(0.0f, y_t, 0.0f);
	draw_balloon(1.0f, 0.0f, 127.0f / 255.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(460.3f, -300.0f, 0.0f);
	glTranslatef(0.0f, y_t, 0.0f);
	draw_balloon(153.0f / 255.0f, 1.0f, 153.0f / 255.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(410.3f, -300.0f, 0.0f);
	glTranslatef(0.0f, y_t, 0.0f);
	draw_balloon(1.0f, 1.0f, 153.0f / 255.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(360.3f, -300.0f, 0.0f);
	glTranslatef(0.0f, y_t, 0.0f);
	draw_balloon(153.0f / 255.0f, 1.0f, 204.0f / 255.0f);
	glPopMatrix();

	// cloud
	glPushMatrix();
	glTranslatef(1035.7f, 745.2f, 0.0f);
	glTranslatef(x_t, 0.0f, 0.0f);
	draw_cloud();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1010.1f, 700.5f, 0.0f);
	glTranslatef(x_t, 0.0f, 0.0f);
	draw_cloud();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(728.7f, 706.4f, 0.0f);
	glTranslatef(x_t, 0.0f, 0.0f);
	draw_cloud();
	glPopMatrix();

	glPushMatrix();
	glScalef(1.5f, 1.5f, 0.0f);
	glTranslatef(630.5f, 190.1f, 0.0f);
	glTranslatef(x_t, 0.0f, 0.0f);
	draw_cloud();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1028.3f, 230.3f, 0.0f);
	glTranslatef(x_t, 0.0f, 0.0f);
	draw_cloud();
	glPopMatrix();
}

// Method to define openGL
void define_to_openGL(void)
{
	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set orthographic projection parameters (left, right, bottpm, top)
	gluOrtho2D(0, 1100, 0, 780);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (flag == true)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	// Smooth effect
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	draw_background();
	draw_rainbow();
	draw_center_building();
	draw_international_research_center();
	draw_word();
	dynamic_scene();

	glutSwapBuffers();
}

// The main function
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1100, 780);
	glutCreateWindow("The Greeting Card for the 15th Anniversary of XJTLU");

	glutDisplayFunc(define_to_openGL);

	glutIdleFunc(when_in_mainloop);
	glutTimerFunc(time_interval, on_y_timer, 1);
	glutTimerFunc(time_interval, on_x_timer, 1);

	glutKeyboardFunc(keyboard_input);

	glutMouseFunc(mouse_input);

	glutMainLoop();
}