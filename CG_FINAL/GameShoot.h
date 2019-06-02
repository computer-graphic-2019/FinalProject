#ifndef GAME_SHOOT_H
#define GAME_SHOOT_H

class GameShoot {
private:

public:
    // 控制开枪（鼠标左键）
    void Fire();
    // 检查命中位置
    void CheckCollisionWithTarget();
    // 计算命中得分
    void CalculateScore();
    // 计算是否需要装弹（非必需）
    void CheckIfNeedReload();
}

#endif