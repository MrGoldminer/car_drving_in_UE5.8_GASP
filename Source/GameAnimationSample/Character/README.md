# Character — 人物玩法模块

人物侧的通用玩法组件与框架接入层。人物本体为官方示例蓝图（`SandboxCharacter_CMC` / `SandboxCharacter_Mover`），本模块不修改蓝图，全部通过组件扩展。

## 类结构

| 类 | 职责 |
| --- | --- |
| `UGameplaySubsystem` | 框架接入层（WorldSubsystem）：每 0.5s 扫描玩家 Pawn，动态注册抓钩、载具交互、武器管理、血量组件；为本地玩家创建血条 HUD；首次运行时生成 2 个演示敌人。开局若玩家直接控制着无司机的 Buggy，自动切回场景人物（排除敌人） |
| `UHealthComponent` | 血量：`MaxHealth/CurrentHealth`，监听 `OnTakeAnyDamage` 扣血（下限 0），致死时广播 `OnDeath` |
| `AEnemyTarget` | 演示敌人（几何体小人 + `UHealthComponent`）：可被刀/枪打掉血，死亡后隐藏并关闭碰撞 |
| `UGrapplingHookComponent` | 抓钩：相机射线找钩点，样条网格绳索表现，按住拉拽（`PullSpeed/PullResponsiveness` 可调），松开释放 |
| `UVehicleInteractionComponent` | 载具交互：按 F 时查找 300cm 内最近的无主 Buggy 上车；驾驶中再按 F 下车切回人物 |

## 输入（Config/DefaultInput.ini）

| Action | 键 | 行为 |
| --- | --- | --- |
| `Grapple` | 鼠标右键 / Q | 按住抓钩拉拽，松开释放 |
| `VehicleInteract` | F | 进入载具 |

## 扩展

新增人物玩法组件：实现 `UActorComponent` 子类（输入在 Tick 中等 `OwnerPawn->InputComponent` 就绪后绑定），并在 `UGameplaySubsystem::Tick` 中注册即可。
