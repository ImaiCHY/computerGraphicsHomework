#define WindowWidth  500
#define WindowHeight 500
#define BMP_Header_Length 54
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <GL/glut.h>

using namespace std;


static float c = 3.1415926 / 180.0f;
static float r = 1.0f;
static int degree = 90;
static int degree_y = 90;
static int oldPosY = -1;
static int oldPosX = -1;
static int autoDo = 0;
static float keyboardI = 1.0f;


static bool hezi = false;
static bool measure = false;
static bool texture = false;
static bool rotate_B = false;
static bool theSun = false;
static GLint theDay = 200;

static GLfloat max[3];
static GLfloat min[3];
static GLfloat v1Sets[15003];
static GLfloat v2Sets[15003];
static GLfloat v3Sets[15003];
static GLfloat area = 0;
static GLfloat volume = 0;

static vector<vector<GLfloat>>vSets;//��Ŷ���(x,y,z)����
static vector<vector<GLfloat>>vnSets;//��Ŷ��㷨����
static vector<vector<GLint>>fSets;//������������������

string filename = "F:\\ѧУ\\������\\Сѧ��\\homework\\bunny.obj";
GLuint bunnyGround;
void ObjLoader() {
    string line;
    fstream f;
    f.open(filename, ios::in);
    if (!f.is_open()) {
        cout << "Something Went Wrong When Opening Objfiles" << endl;

    }

    while (!f.eof()) {

        getline(f, line);//�õ�obj�ļ���һ�У���Ϊһ���ַ���
        vector<string>parameters;
        string tailMark = " ";
        string ans = "";
        //cout << line << endl;
        line = line.append(tailMark);
        for (int i = 0; i < line.length(); i++) {
            char ch = line[i];
            //cout << ch << endl;
            if (ch != ' ') {
                ans += ch;
            }
            else {
                parameters.push_back(ans); //ȡ���ַ����е�Ԫ�أ��Կո��з�
                ans = "";
            }
        }
        //cout << parameters.size() << endl;
        if (parameters.size() != 4) {
            //cout << "the size is not correct" << endl;
        }
        else {
            if (parameters[0] == "vn") {   //����Ƕ���Ļ�
                vector<GLfloat>Point;
                for (int i = 1; i < 4; i++) {   //��1��ʼ���������xyz����������뷨����
                    GLfloat xyz = atof(parameters[i].c_str());
                    /*cout << xyz << endl;*/
                    Point.push_back(xyz);
                }

                vnSets.push_back(Point);
            }
            else if (parameters[0] == "v") {
                vector<GLfloat>Point;
                for (int i = 1; i < 4; i++) {   //��1��ʼ���������xyz����������붥��vector
                    GLfloat xyz = atof(parameters[i].c_str());
                    /*cout << parameters[i].c_str() << endl;*/
                    Point.push_back(xyz);
                }
                vSets.push_back(Point);
            }

            //if (parameters[0] == "v") {   //����Ƕ���Ļ�
            //        
            //            vector<GLfloat>Point;
            //            for (int i = 1; i < 4; i++) {   //��1��ʼ���������xyz����������뷨����
            //                GLfloat xyz = atof(parameters[i].c_str());
            //                Point.push_back(xyz);
            //            }
            //            vSets.push_back(Point);
            //        
            //}

            else if (parameters[0] == "f") {   //�������Ļ�������������������
                vector<GLint>vIndexSets;
                for (int i = 1; i < 4; i++) {
                    string x = parameters[i];
                    string ans = "";
                    for (int j = 0; j < x.length(); j++) {   //������/��
                        char ch = x[j];
                        if (ch != '/') {
                            ans += ch;
                        }
                        else {
                            break;
                        }
                    }
                    GLint index = atof(ans.c_str());
                    index = index--;//��Ϊ����������obj�ļ����Ǵ�1��ʼ�ģ������Ǵ�ŵĶ���vector�Ǵ�0��ʼ�ģ����Ҫ��1
                    vIndexSets.push_back(index);
                }
                fSets.push_back(vIndexSets);
            }
        }
    }
    f.close();

}
int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}


/* ����load_texture
 * ��ȡһ��BMP�ļ���Ϊ����
 * ���ʧ�ܣ�����0������ɹ�������������
 */
GLuint load_texture(const char* file_name)
{
    GLint width, height, total_bytes;
    GLubyte* pixels = 0;
    // GLuint last_texture_ID, texture_ID = 0;
    GLint last_texture_ID;
    GLuint    texture_ID = 0;

    // ���ļ������ʧ�ܣ�����
    FILE* pFile = fopen(file_name, "rb");
    if (pFile == 0)
        return 0;

    // ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

    // ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    // �����������ֽ��������ڴ�
    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }

    // ��ȡ��������
    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // �ھɰ汾��OpenGL��
    // ���ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������
    // ���ﲢû�м��OpenGL�汾�����ڶ԰汾�����ԵĿ��ǣ����ɰ汾����
    // ���⣬�����Ǿɰ汾�����°汾��
    // ��ͼ��Ŀ�Ⱥ͸߶ȳ�����ǰOpenGLʵ����֧�ֵ����ֵʱ��ҲҪ��������
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max)
        {
            const GLint new_width = 24;
            const GLint new_height = 24; // �涨���ź��µĴ�СΪ�߳���������
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;

            // ����ÿ����Ҫ���ֽ��������ֽ���
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            // �����ڴ�
            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }

            // ������������
            gluScaleImage(GL_RGB,
                width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            // �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height
            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    // ����һ���µ�������
    glGenTextures(1, &texture_ID);
    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        
        return 0;
    }

    // ���µ������������������������
    // �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�
    //glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);

    // ֮ǰΪpixels������ڴ����ʹ��glTexImage2D�Ժ��ͷ�
    // ��Ϊ��ʱ���������Ѿ���OpenGL���б�����һ�ݣ����ܱ����浽ר�ŵ�ͼ��Ӳ���У�
    free(pixels);
    return texture_ID;
}




void suofang(float a) {
    keyboardI = a;
    measure = false;
    area = 0;
    volume = 0;
}

void Swap(vector<GLfloat> a, int i, int j) {
    float temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

void maopao(GLfloat a[]) {


    int i; int j; float temp;
    for (i = 0; i < 15002; i++)
        for (j = 0; j < 15002 - i; j++)
            if (a[j] > a[j + 1])
            {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }

}






//���ù�Դ
void setLightRes() {
    //GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 0.8f };

    //glLightfv(GL_LIGHT0, GL_AMBIENT, lightPosition);
    glEnable(GL_LIGHTING); //���ù�Դ
    glEnable(GL_LIGHT0);   //ʹ��ָ���ƹ�
}









void init() {
    ObjLoader();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("����");
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    setLightRes();
    

    
    
}

void display(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);


    
    setLightRes();




    glPushMatrix();


    if (!theSun) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gluLookAt(r * cos(c * (degree + autoDo)), r * sin(c * (degree_y + autoDo)), r * sin(c * (degree + autoDo)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    }
    else {
        glDisable(GL_BLEND);
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 1.5f, 0.0f);
        glTranslatef(0.0f, -1.7f, 0.0f);
        glutSolidSphere(0.1f, 20, 20);
        glTranslatef(0.0f, 1.7f, 0.0f);
        gluLookAt(-5.0f, 0.0f , 0.0f ,
            0.0f,0.0f, 0.0f, 0.0f, 2.0f, 0.0f);

        glTranslatef(-2.0f, -2.0f, 0.0f);
        glRotatef(theDay / 360.0 * 360.0, 1.0f, 0.0f,0.0f);
        glTranslatef(2.0f, 2.0f, 0.0f);
    }
    
    
    






  



    //Draw();
    //����objģ��
    int sizeF = fSets.size() - 1;

    int tmp_x = 0;
    
    if (!texture) {
        glDisable(GL_TEXTURE_2D);
    }
    else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bunnyGround);
    }
    
    glBegin(GL_TRIANGLES);//��ʼ����

    for (int i = 0; i < fSets.size(); i++) {
        GLfloat VN[3];
        //��������
        GLfloat SV1[3];
        GLfloat SV2[3];
        GLfloat SV3[3];




        if ((fSets[i]).size() != 3) {
            cout << "the fSetsets_Size is not correct" << endl;
        }

        else {
            GLint firstVertexIndex = (fSets[i])[0];//ȡ����������
            GLint secondVertexIndex = (fSets[i])[1];
            GLint thirdVertexIndex = (fSets[i])[2];

            SV1[0] = (vSets[firstVertexIndex])[0];//��һ������
            SV1[1] = (vSets[firstVertexIndex])[1];
            SV1[2] = (vSets[firstVertexIndex])[2];

            SV2[0] = (vSets[secondVertexIndex])[0]; //�ڶ�������
            SV2[1] = (vSets[secondVertexIndex])[1];
            SV2[2] = (vSets[secondVertexIndex])[2];

            SV3[0] = (vSets[thirdVertexIndex])[0]; //����������
            SV3[1] = (vSets[thirdVertexIndex])[1];
            SV3[2] = (vSets[thirdVertexIndex])[2];

            if (!measure) {
                //��������
                GLfloat edgeX;
                GLfloat edgeY;
                GLfloat edgeZ;

                edgeX = 20 * keyboardI * sqrt(pow(SV2[0] - SV1[0], 2) + pow(SV2[1] - SV1[1], 2) +
                    pow(SV2[2] - SV1[2], 2));
                edgeY = 20 * keyboardI * sqrt(pow(SV3[0] - SV1[0], 2) + pow(SV3[1] - SV1[1], 2) +
                    pow(SV3[2] - SV1[2], 2));
                edgeZ = 20 * keyboardI * sqrt(pow(SV2[0] - SV3[0], 2) + pow(SV2[1] - SV3[1], 2) +
                    pow(SV2[2] - SV3[2], 2));

                //���׹�ʽ

                GLfloat areaP = (edgeX + edgeY + edgeZ) / 2;
                area = area + sqrt(areaP * (areaP - edgeX) * (areaP - edgeY) * (areaP - edgeZ));

                //������� ���ü�����������з����������
                GLfloat v321 = (20 * keyboardI * SV3[0]) * (20 * keyboardI * SV2[1] - 2) * (20 * keyboardI * SV1[2]);
                GLfloat v231 = (20 * keyboardI * SV2[0]) * (20 * keyboardI * SV3[1] - 2) * (20 * keyboardI * SV1[2]);
                GLfloat v312 = (20 * keyboardI * SV3[0]) * (20 * keyboardI * SV1[1] - 2) * (20 * keyboardI * SV2[2]);
                GLfloat v132 = (20 * keyboardI * SV1[0]) * (20 * keyboardI * SV3[1] - 2) * (20 * keyboardI * SV2[2]);
                GLfloat v213 = (20 * keyboardI * SV2[0]) * (20 * keyboardI * SV1[1] - 2) * (20 * keyboardI * SV3[2]);
                GLfloat v123 = (20 * keyboardI * SV1[0]) * (20 * keyboardI * SV2[1] - 2) * (20 * keyboardI * SV3[2]);

                volume = volume + abs(((1.0f / 6.0f) *
                    (-v321 + v231 + v312 - v132 - v213 + v123)));

            }



            v1Sets[tmp_x] = SV1[0];
            v2Sets[tmp_x] = SV1[1];
            v3Sets[tmp_x] = SV1[2];
            tmp_x++;
            v1Sets[tmp_x] = SV2[0];
            v2Sets[tmp_x] = SV2[1];
            v3Sets[tmp_x] = SV2[2];
            tmp_x++;
            v1Sets[tmp_x] = SV3[0];
            v2Sets[tmp_x] = SV3[1];
            v3Sets[tmp_x] = SV3[2];
            tmp_x++;


            GLfloat vec1[3], vec2[3], vec3[3], vec4[4];//���㷨����
            //(x2-x1,y2-y1,z2-z1)
            vec1[0] = SV1[0] - SV2[0];
            vec1[1] = SV1[1] - SV2[1];
            vec1[2] = SV1[2] - SV2[2];

            //(x3-x2,y3-y2,z3-z2)
            vec2[0] = SV1[0] - SV3[0];
            vec2[1] = SV1[1] - SV3[1];
            vec2[2] = SV1[2] - SV3[2];

            //(x3-x1,y3-y1,z3-z1)
            vec3[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
            vec3[1] = vec2[0] * vec1[2] - vec2[2] * vec1[0];
            vec3[2] = vec2[1] * vec1[0] - vec2[0] * vec1[1];

            GLfloat D = sqrt(pow(vec3[0], 2) + pow(vec3[1], 2) + pow(vec3[2], 2));

            VN[0] = vec3[0] / D;
            VN[1] = vec3[1] / D;
            VN[2] = vec3[2] / D;






            GLfloat diffuse[] = { 0.5f,0.5f,0.5f,0.5f };
            GLfloat shiness[] = { 60.0f };
            GLfloat emission[] = { 0.5f,0.05f,0.05f,1.0f };
            GLfloat AandD[] = { 0.5f,0.5f,0.5f,0.8f };
            GLfloat specular[] = { 0.5f,0.5f,0.5f,0.7f };

            glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
            glMaterialfv(GL_FRONT,GL_SHININESS,shiness);
            glMaterialfv(GL_FRONT,GL_EMISSION,emission);
            glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,AandD);
            glMaterialfv(GL_FRONT,GL_SPECULAR,specular);//����




            glNormal3f(VN[0], VN[1], VN[2]);//���Ʒ�����
                
                if (!texture) {
                    
                    
                    glVertex3f(SV1[0] * 20 * keyboardI, SV1[1] * 20 * keyboardI - 2, SV1[2] * 20 * keyboardI);//����������Ƭ
                    
                    
                    glVertex3f(SV2[0] * 20 * keyboardI, SV2[1] * 20 * keyboardI - 2, SV2[2] * 20 * keyboardI);
                    
                    glVertex3f(SV3[0] * 20 * keyboardI, SV3[1] * 20 * keyboardI - 2, SV3[2] * 20 * keyboardI);
                }
                else {
                    glTexCoord2f(0.0f, 0.0f);

                    glVertex3f(SV1[0] * 20 * keyboardI, SV1[1] * 20 * keyboardI - 2, SV1[2] * 20 * keyboardI);//����������Ƭ
                    glTexCoord2f(0.0f, 5.0f);
                    glVertex3f(SV2[0] * 20 * keyboardI, SV2[1] * 20 * keyboardI - 2, SV2[2] * 20 * keyboardI);
                    glTexCoord2f(5.0f, 5.0f);
                    glVertex3f(SV3[0] * 20 * keyboardI, SV3[1] * 20 * keyboardI - 2, SV3[2] * 20 * keyboardI);
                }
            
   

            
            

           


        }

        if (!measure) {
            cout << "��ʱ�����Ϊ" << area << endl;
            cout << "��ʱ���Ϊ" << abs(volume) << endl;
            
            measure = true;

        }
    }



        glEnd();






        



        //DrawHe();
        //����Χ��
        if (!hezi) {
            maopao(v1Sets);
            maopao(v2Sets);
            maopao(v3Sets);



            min[0] = v1Sets[0];
            min[1] = v2Sets[0];
            min[2] = v3Sets[0];




            max[0] = v1Sets[sizeof(v1Sets) / sizeof(v1Sets[0]) - 1];
            max[1] = v2Sets[sizeof(v2Sets) / sizeof(v2Sets[0]) - 1];
            max[2] = v3Sets[sizeof(v3Sets) / sizeof(v3Sets[0]) - 1];



            hezi = true;
        }



        if (!theSun) {
            glDepthMask(GL_FALSE);
            glBegin(GL_QUADS);




            GLfloat mat_ambient[] = { 0.105882, 0.058824, 0.113725, 0.400000 };

            GLfloat mat_diffuse[] = { 0.427451, 0.470588, 0.541176, 0.400000 };

            GLfloat mat_specular[] = { 0.333333, 0.333333, 0.521569, 0.400000 };

            GLfloat mat_shininess[] = { 9.846150 }; //����RGBA����ָ������ֵ��0��128��Χ��

            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

            glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a1
            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a2
            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a4
            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a3

            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a5
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a6
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a8
            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a7

            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a3
            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a4
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a8
            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a7

            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a1
            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a2
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a6
            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a5

            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a1
            glVertex3f(min[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a3
            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a7
            glVertex3f(min[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a5

            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a2
            glVertex3f(max[0] * 20 * keyboardI, min[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a4
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, max[2] * 20 * keyboardI);//a8
            glVertex3f(max[0] * 20 * keyboardI, max[1] * 20 * keyboardI - 2, min[2] * 20 * keyboardI);//a6


            glEnd();

            glDepthMask(GL_TRUE);



            glPopMatrix();
        }



        



        glutSwapBuffers();
        glFlush();
    }

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLdouble)width / (GLdouble)height, 1.0f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
}

//�ƶ����360�۲�ģ��
void mouseMove(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        oldPosX = x; oldPosY = y;
    }
}
void changeViewPoint(int x, int y)
{
    int temp = x - oldPosX;
    degree += temp;
    int temp_y = y - oldPosY;
    degree_y += temp_y;
    oldPosX = x;
    oldPosY = y;
}

void keyboardChange(unsigned char key, int x, int y) {
    switch (key) {
    case 'w'://�Ŵ�
        if (!theSun) {
            keyboardI = keyboardI + 0.2;
            suofang(keyboardI);
            break;
        }
        break;
    case 's'://��С
        if (!theSun) {
            if (keyboardI <= 0.5) {
                break;
            }
            keyboardI = keyboardI - 0.5;
            suofang(keyboardI);
            break;
        }
        break;
    case 'a'://��������
        texture = !texture;
        break;
    case 'd'://�Զ���ת
        rotate_B = !rotate_B;
        break;
    
    case 'f'://�Ʋ�������ת
        theSun = !theSun;
        break;
    }
    
}

void myIdle()
{
    if (!rotate_B) {
        
    }
    else {
        if (!theSun) {
            ++autoDo;
            if (autoDo >= 360)
                autoDo = 0;
        }
        else {
            autoDo = 0;
            ++theDay;

            if (theDay >= 360) {
                theDay = 0;
            }
        }
    }
    
    /*myDisplay();*/
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(&display);
    glutReshapeFunc(&reshape);
    glutMouseFunc(&mouseMove);
    glutKeyboardFunc(&keyboardChange);
    glutMotionFunc(&changeViewPoint);
    glutIdleFunc(&myIdle);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    bunnyGround = load_texture("F:\\ѧУ\\������\\Сѧ��\\9-10\\1.bmp");
   
    glutMainLoop();


}
