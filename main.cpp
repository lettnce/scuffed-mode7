#include <cstdio>
#include "olcConsoleGameEngine.h"

class FakeMode7 : public olcConsoleGameEngine {
    public:
        FakeMode7() {
            m_sAppName = L"Pseudo 3D Planes";
        }
    
    private:
        olcSprite *sprGround; //world map we draw frustum from
        int nMapSize = 1024;

        //where player is in world
        float fWorldX = 0.0f;
        float fWorldY = 0.0f;
        float fWorldA = 0.0f; //angle
        
        //frustum
        float fNear = 0.01f;
        float fFar = 100.0f;
        float fFoVHalf = 3.14159f / 4.0f;

    protected:
        virtual bool OnUserCreate() { //the program is first created, so we must create the world
            sprGround = new olcSprite(nMapSize, nMapSize); //creating ground 
            for (int x{0}; x < nMapSize; x += 32) {
                for (int y{0}; y < nMapSize; y++) {
                    //draw in y
                    sprGround->SetColour(x, y, FG_MAGENTA);
                    sprGround->SetGlyph(x, y, PIXEL_SOLID); //pixel type
                    sprGround->SetColour(x + 1, y, FG_MAGENTA); //neighboring pixels
                    sprGround->SetGlyph(x + 1, y, PIXEL_SOLID);
                    sprGround->SetColour(x - 1, y, FG_MAGENTA);
                    sprGround->SetGlyph(x - 1, y, PIXEL_SOLID);
                    //draw in x
                    sprGround->SetColour(y, x, FG_BLUE);
                    sprGround->SetGlyph(y, x, PIXEL_SOLID);
                    sprGround->SetColour(y, x + 1, FG_BLUE);
                    sprGround->SetGlyph(y, x + 1, PIXEL_SOLID);
                    sprGround->SetColour(y, x - 1, FG_BLUE);
                    sprGround->SetGlyph(y, x - 1, PIXEL_SOLID);
                }
            }
             return true;
        }
        virtual bool OnUserUpdate(float fElapsedTime) { //probably means when we "update" the program i.e. send an input \

            if (GetKey(L'Q').bHeld) fNear += 0.1f * fElapsedTime;
            if (GetKey(L'A').bHeld) fNear -= 0.1f * fElapsedTime;
            
            if (GetKey(L'W').bHeld) fFar += 0.1f * fElapsedTime;
            if (GetKey(L'S').bHeld) fFar -= 0.1f * fElapsedTime;
            
            if (GetKey(L'Z').bHeld) fFoVHalf += 0.1f * fElapsedTime;
            if (GetKey(L'X').bHeld) fFoVHalf -= 0.1f * fElapsedTime;
     
            float fFarX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
            float fFarY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

            float fFarX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
            float fFarY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

            float fNearX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
            float fNearY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

            float fNearX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
            float fNearY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

            for (int y{0}; y < ScreenHeight() / 2; y++) {
                float fSampleDepth = (float)y / ((float)ScreenHeight() / 2.0f);

                float fStartX = (fFarX1 - fNearX1) * (fSampleDepth) + fNearX1;                
                float fStartY = (fFarY1 - fNearY1) * (fSampleDepth) + fNearY1;
                float fEndX = (fFarX2 - fNearX2) * (fSampleDepth) + fNearX2;                
                float fEndY = (fFarY2 - fNearY2) * (fSampleDepth) + fNearY2;

                for (int x{0}; x < ScreenWidth(); x++) {
                    float fSampleWidth = (float)x / (float)ScreenWidth();
                    float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
                    float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

                    fSampleX = fmod(fSampleX, 1.0f);
                    fSampleY = fmod(fSampleY, 1.0f);

                    wchar_t sym = sprGround->GetGlyph((int)fSampleX, (int)fSampleY);
                    short col = sprGround->GetColour((int)fSampleX, (int)fSampleY);

                    Draw(x, y + (ScreenHeight() / 2), sym, col);
                }

                if (GetKey(VK_LEFT).bHeld) {
                    fWorldA -= 1.0f * fElapsedTime;
                }
                if (GetKey(VK_RIGHT).bHeld) {
                    fWorldA += 1.0f * fElapsedTime;
                }

                if (GetKey(VK_UP).bHeld) {
                    fWorldX += cosf(fWorldA) * 0.2f * fElapsedTime;
                    fWorldY += sinf(fWorldA) * 0.2f * fElapsedTime;
                }
                if (GetKey(VK_DOWN).bHeld) {
                    fWorldX -= cosf(fWorldA) * 0.2f * fElapsedTime;
                    fWorldY -= sinf(fWorldA) * 0.2f * fElapsedTime;
                }
            }

            return true;
        }
};

int main() {

    FakeMode7 game;
    game.ConstructConsole(80, 50, 12, 12);
    game.Start();

    return 0;
}
