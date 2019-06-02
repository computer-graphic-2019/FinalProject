#ifndef GAME_MOVE_H
#define GAME_MOVE_H

class GameMove {
private:
    bool isGunRaiseUp = false;
public:
    // 控制人物前移(w)
    void humanMoveForward();
    // 控制人物后退(s)
    void humanMoveBackward();
    // 控制人物左转(a)
    void humanMoveLeft();
    // 控制人物右转(d)
    void humanMoveRight();
    // 人物移动总控制
    void humanMove();
    // 控制举枪（右键开镜）
    void raiseUpGun();
    // 控制收枪（右键关镜）
    void putDownGun();
    // 枪械动作总控制
    void gunMove();
}

#endif