# Character — 人物玩法模块

人物侧的通用玩法组件与框架接入层。人物本体为官方示例蓝图（`SandboxCharacter_CMC` / `SandboxCharacter_Mover`），本模块不修改蓝图，全部通过组件扩展。

## 类结构

| 类 | 职责 |
| --- | --- |
| `UGameplaySubsystem` | 框架接入层（WorldSubsystem）：每 0.5s 扫描玩家 Pawn，动态注册抓钩、载具交互、武器管理组件；载具 Pawn 跳过 |
| `UGrapplingHookComponent` | 抓钩：相机射线找钩点，样条网格绳索表现，按住拉拽（`PullSpeed/PullResponsiveness` 可调），松开释放 |
| `UVehicleInteractionComponent` | 载具交互：按 F 时查找 300cm 内最近的无主 Buggy 上车；驾驶中再按 F 下车切回人物 |

## 输入（Config/DefaultInput.ini）

| Action | 键 | 行为 |
| --- | --- | --- |
| `Grapple` | 鼠标右键 / Q | 按住抓钩拉拽，松开释放 |
| `VehicleInteract` | F | 进入载具 |

## 扩展

新增人物玩法组件：实现 `UActorComponent` 子类（输入在 Tick 中等 `OwnerPawn->InputComponent` 就绪后绑定），并在 `UGameplaySubsystem::Tick` 中注册即可。
