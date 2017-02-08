// ffmpegDecoder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ffmpegDecode.h"
#include <windows.h>
#include <stdlib.h>    // for atoi
#include "SDL.h"
#include "SDL_main.h"

#define FILENAME          "C:\\fizboi\\video.mp4"
#define OUTPUT_FILE_PREFIX "c:\\fizboi\\image%d.bmp"
#define FRAME_COUNT        9999999

#include <iostream>
#include <fstream>
using namespace std;
#include <string>

SDL_Surface* screen;
Uint16 * Adress[2048][2048];

Uint16 Background[2048][2048];

SDL_Event eventing;

int W=272;
int H=192;
float radius=12.0f;
float delta=60;
int DELAY=0;

bool BMPSave(const char *pFileName, AVFrame * frame, int w, int h)
{
	bool bResult = false;

	if (frame)
	{
		FILE* file = fopen(pFileName, "wb");

		if (file)
		{
			// RGB image
			int imageSizeInBytes = 3 * w * h;
			int headersSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			int fileSize = headersSize + imageSizeInBytes;

			uint8_t * pData = new uint8_t[headersSize];

			if (pData != NULL)
			{
				BITMAPFILEHEADER& bfHeader = *((BITMAPFILEHEADER *)(pData));

				bfHeader.bfType = 'MB';
				bfHeader.bfSize = fileSize;
				bfHeader.bfOffBits = headersSize;
				bfHeader.bfReserved1 = bfHeader.bfReserved2 = 0;

				BITMAPINFOHEADER& bmiHeader = *((BITMAPINFOHEADER *)(pData + headersSize - sizeof(BITMAPINFOHEADER)));

				bmiHeader.biBitCount = 3 * 8;
				bmiHeader.biWidth    = w;
				bmiHeader.biHeight   = h;
				bmiHeader.biPlanes   = 1;
				bmiHeader.biSize     = sizeof(bmiHeader);
				bmiHeader.biCompression = BI_RGB;
				bmiHeader.biClrImportant = bmiHeader.biClrUsed = 
					bmiHeader.biSizeImage = bmiHeader.biXPelsPerMeter = 
					bmiHeader.biYPelsPerMeter = 0;

				fwrite(pData, headersSize, 1, file);

				uint8_t *pBits = frame->data[0] + frame->linesize[0] * h - frame->linesize[0];
				int nSpan      = -frame->linesize[0];

				int numberOfBytesToWrite = 3 * w;

				for (size_t i = 0; i < h; ++i, pBits += nSpan)	
				{
					fwrite(pBits, numberOfBytesToWrite, 1, file);
				}

				bResult = true;
				delete [] pData;				
			}

			fclose(file);
		}
	}

	return bResult;
}







void Index(int w, int h)
{
	Uint8 * localm;
		localm=(Uint8 *)screen->pixels;
		for (int n=0; n<w; n++) {
		for (int m=0; m<h; m++) {
		Adress[n][m]=(Uint16 *)(localm + (n*2+m*screen->pitch));
		//Background[n][m]=(Uint16 *)(localm + (n*2+m*screen->pitch));
		}
		}
}





bool ColorMatch(Uint16 Picked, Uint16 Needed)
{
	Uint8 RP,GP,BP;
	Uint8 RN,GN,BN;

	SDL_GetRGB(Picked,screen->format,&RP,&GP,&BP);
	SDL_GetRGB(Needed,screen->format,&RN,&GN,&BN);

	float rp=RP,gp=GP,bp=BP;
	float rn=RN,gn=GN,bn=BN;

	if(abs(rp-rn)<delta && abs(gp-gn)<delta && abs(bp-bn)<delta)
	return true;
	
	return false;
}


void PickBG()
{		int n;int m;
		for (n=0; n<W; n++) {
			for (m=0; m<H; m++) {
				Background[n][m]=*Adress[n][m];
			}
		}
}


int main(int argc, char* argv[])
{

  printf("Usage: %s filename\n\n\n", argv[0]);

  //string line;
  //ifstream myfile ("settings.txt");
  //if (myfile.is_open())
  //{
  //  while ( getline (myfile,line) )
  //  {
  //    cout << line << endl;
  //  }
  //  myfile.close();
  //}
  //else cout << "Unable to open settings file\n";


  cout << "Zagryzka video C:\\fizboi\\video.mp4 \n";


	FFmpegDecoder decoder;

  if (decoder.OpenFile(std::string(FILENAME)))
  {
    int w = decoder.GetWidth();
    int h = decoder.GetHeight();
	W=w;
	H=h;

  cout << "\n";

  cout << "FPS? (default 29). (kadrov v sekundy) \n";
  int FPS;
  cin >> FPS;
  cout << "\n";
  int FWD;

  if(FPS==1)
  {
	FPS=240;delta=50;DELAY=0;FWD=240;		
  }
  else
  {
  cout << "MAX.DELTA? (default 60). (bolshe znachenie => menshe chuvstvitelnost') \n";
  int Delta;
  cin >> Delta;
  delta=Delta;
  cout << "\n";

  cout << "Delay? (default 0). (zaderzhka dlya proverki) \n";
  cin >> DELAY;
  cout << "\n";

  cout << "Fwd Speed? (default 60). (zaderzhka dlya proverki) \n";
  
  cin >> FWD;
  cout << "\n";
  }

	SDL_Init( SDL_INIT_EVERYTHING );

	screen = SDL_SetVideoMode( w, h, 16, SDL_HWSURFACE);//SDL_FULLSCREEN );//

	Index(w,h);

	SDL_WM_SetCaption( "FIZBOI", "Ихихи" );

	cout << "LMB - START, RMB - FORWARD, MMB - RADIUS (knopki mishi i kolesiko) \n";

	bool go=false;

	decoder.GetNextFrame();decoder.GetNextFrame();decoder.GetNextFrame();
	AVFrame * frame2 = decoder.GetNextFrame();
	

	int AIMX, AIMY;
	int MX,MY;
	Uint32 Good;

		while(go==false)
		{

	SDL_GetMouseState(&MX,&MY);

		while(SDL_PollEvent(&eventing)){
			switch(eventing.type){
			case SDL_MOUSEBUTTONDOWN:
				switch( eventing.button.button )
				{

				case SDL_BUTTON_LEFT:
				go=true;	
				break;

				case SDL_BUTTON_WHEELUP:
				radius++;	
				break;

				case SDL_BUTTON_WHEELDOWN:
				radius--;	
				break;

				case SDL_BUTTON_RIGHT:

					av_free(frame2->data[0]);
					av_free(frame2);

					for(int a=0;a<FWD;a++) {
					frame2 = decoder.GetNextFrame();
					av_free(frame2->data[0]);
					av_free(frame2);
					}
					frame2 = decoder.GetNextFrame();

				break;

				default: break;
				}
			break;
			}
		}

		  
		  if (frame2)
		  {
			  float dist;
 			for (int y=1; y<H; y++) {
				for (int x=1; x<W; x++) {
					int p=x*3+y*frame2->linesize[0];
					*Adress[x][y]=SDL_MapRGB(screen->format,frame2->data[0][p+2],frame2->data[0][p+1],frame2->data[0][p]);
					dist=sqrt(pow(x-MX,2.0)+pow(y-MY,2.0));
					if(dist<radius+4.0f && dist>radius)
						*Adress[x][y]=SDL_MapRGB(screen->format,0,255,0);
				}
			}
			SDL_Flip(screen);
		  }

		}

		PickBG();

		AIMX=MX;AIMY=MY;
		 // if(black)
		//	  Good=0;
		 // else
		Good=*Adress[MX][MY];

					int p=MX*3+MY*frame2->linesize[0];
					//*Adress[x][y]=SDL_MapRGB(screen->format,,,);

			Uint8 r=frame2->data[0][p+2],g=frame2->data[0][p+1],b=frame2->data[0][p];
			SDL_GetRGB(Good,screen->format,&r,&g,&b);

			av_free(frame2->data[0]);
			av_free(frame2);

			int REST; bool BINGO;


	//ofstream myfile ("C:/fizboi/output.txt");
 // if (myfile.is_open())
 // {
 //   myfile << "This is a line.\n";
 //   myfile << "This is another line.\n";
 // }else cout << "Unable to open txt output file";


      std::ofstream o("C:/fizboi/output.txt");

      //o << "Hello, World\n" << std::endl;
	float StartTime=0.0f;
	AVFrame * frame;

    for (int i = 0; i < FRAME_COUNT; i++)
    {


//		decoder.GetNextFrame();decoder.GetNextFrame();decoder.GetNextFrame();
      frame = decoder.GetNextFrame();
      
	  if (frame)
      {
        char filename[MAX_PATH];
        sprintf(filename, OUTPUT_FILE_PREFIX, i);

		//if (!BMPSave(filename, frame, frame->width, frame->height))
  //      {
  //        printf("Cannot save file %s\n", filename);
  //      }
		
			

 		for (int y=1; y<H; y++) {
			for (int x=1; x<W; x++) {
				int p=x*3+y*frame->linesize[0];
				
				
				
				
				//if(rand()%2==0)
				
				*Adress[x][y]=SDL_MapRGB(screen->format,frame->data[0][p+2],frame->data[0][p+1],frame->data[0][p]);
				
				//if(ColorMatch(*Adress[x][y],Background[x][y]))
				//*Adress[x][y]=10;//*Background[x][y];




			}
		}

		if(ColorMatch(*Adress[AIMX][AIMY],Good)
			&& ColorMatch(*Adress[AIMX+(int)radius][AIMY],Good)
				&& ColorMatch(*Adress[AIMX-(int)radius][AIMY],Good)
					&& ColorMatch(*Adress[AIMX][AIMY+(int)radius],Good)
						&& ColorMatch(*Adress[AIMX][AIMY-(int)radius],Good))
		{
			//REST--;
			//if(REST<4)
			//{



			if(BINGO==false)
			{

			if(StartTime==0)
				StartTime=(1.0/FPS)*i;

			BINGO=true;
			REST=4;
			
			printf ("BINGO Frame# %ld\n", i );
			printf ("Time# %4.3f\n", (1.0/FPS)*i-1.0f*StartTime );
			      //std::ofstream o("C:/fizboi/output.txt");

			o << ((1.0/FPS)*i-1.0f*StartTime) << std::endl;

			//o << "\n" << std::endl;

//if (myfile.is_open())
//{
//    myfile << "%ld\n",(1.0/FPS)*i;
//	 myfile << "HELLO",(1.0/FPS)*i;
//}
	    //o << (1.0/FPS)*i;// << std::endl;
		//o << "\n";

			}
			//}
		}
		else BINGO=false;

		//printf ("Frame# %ld\n", i );
		SDL_Flip(screen);
		SDL_Delay(DELAY);

		av_free(frame->data[0]);
        av_free(frame);

      }

    }
	//myfile.close();
	//o << std::endl;
    decoder.CloseFile();
  }
  else
  {
    printf ("Cannot open file " FILENAME "\n");
	float o;
	cin >> o;
  }

	return 0;
}

