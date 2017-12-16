#pragma once 


/// Вызывается один раз при обнаружении новой флешки.
class FileManager
{
public:
    static void Init();
    
    static void Draw();
    
    static void PressSB_LevelUp();
    
    static void PressSB_LevelDown();
    
    static void RotateRegSet(int angle);
    
    static bool GetNameForNewFile(char name[255]);
    
    static void PressSB_Tab();
};
