# GameAnimationSample

基于 Unreal Engine 的游戏动画与玩法示例项目。在官方动画示例（Mover / CharacterMovement 双角色、跑酷、动画扭曲）之上，扩展了**武器库**与**载具**两大玩法模块。

代码仓参考 Apollo（百度自动驾驶）的模块化组织方式：`Source/GameAnimationSample/` 下每个目录是一个自治模块，模块内附 `README.md` 说明职责、接口与扩展点；跨模块集成集中在 `Character/GameplaySubsystem`（框架接入层）。

## 环境要求

| 项 | 版本 |
| --- | --- |
| Unreal Engine | 5.8 |
| Visual Studio | 2022/2026（MSVC 14.x） |
| 平台 | Win64 |

说明：依赖 5.8 的 `AnimationLayering` 插件与 `BuildSettingsVersion.V7`。若使用 5.7 编译，需从 `.uproject` 移除该插件并将两个 `Target.cs` 降为 `V6`。

## 构建

```bash
"<UE>/Engine/Build/BatchFiles/Build.bat" GameAnimationSampleEditor Win64 Development \
  -Project="<本仓>/GameAnimationSample.uproject" -WaitMutex
```

然后用 `UnrealEditor.exe` 打开 `GameAnimationSample.uproject` 即可运行。

## 模块结构

```
Source/GameAnimationSample/
├── Character/    人物玩法：抓钩、载具交互、GameplaySubsystem（组件接入层）
├── Vehicles/     载具系统：碰碰车物理基类、Buggy、自动泊车、泥地赛道
├── Weapons/      武器库：太刀 / 唐刀（近战）、AK47（射击 + 弹道轨迹）
└── GameAnimationSample.Build.cs
```

各模块详细文档见对应目录下的 `README.md`：

- [Weapons — 武器库](Source/GameAnimationSample/Weapons/README.md)
- [Vehicles — 载具系统](Source/GameAnimationSample/Vehicles/README.md)
- [Character — 人物玩法](Source/GameAnimationSample/Character/README.md)

### 集成方式

`UGameplaySubsystem`（WorldSubsystem）每 0.5s 扫描玩家 Pawn，动态挂载各模块组件（抓钩、载具交互、武器管理）。载具 Pawn 自身跳过挂载。新增人物玩法组件时，只需在该子系统中注册，无需修改人物蓝图。

## 操作说明

| 按键 | 功能 |
| --- | --- |
| W / A / S / D | 移动 / 驾驶 |
| Space | 跳跃 / 载具手刹 |
| 鼠标左键 | 武器攻击（刀类挥砍；AK47 按住连发） |
| 鼠标右键 / Q | 抓钩（按住拉拽，松开释放） |
| Tab | 循环切换武器：徒手 → 太刀 → 唐刀 → AK47 → 徒手 |
| F | 靠近小车上车（300cm 内）；驾驶中下车切回人物 |
| P | 自动泊车开关（驾驶 Buggy 时） |
| R | 载具重置 |

## 武器速览

| 武器 | 类型 | 伤害 | 特性 |
| --- | --- | --- | --- |
| 太刀 Katana | 近战 | 35 | 弧形刀身，挥砍范围 220cm |
| 唐刀 TangDao | 近战 | 35 | 直刃，参数与太刀一致，模型不同 |
| AK47 | 枪械 | 25/发 | 600 RPM 全自动，枪口火光 + 弹道轨迹 |

当前武器模型由引擎基础几何体程序化拼装（见各 `BuildModel()`），可直接在编辑器中替换为正式美术资产；弹道材质通过 `RifleWeapon.TracerMaterial` 配置（建议换成自发光材质）。

## 内容资产

- `Content/Blueprints/`：`SandboxCharacter_CMC` / `SandboxCharacter_Mover` 人物、`GM_Sandbox` 模式
- `Content/Levels/`：`DefaultLevel`（默认）、`BuggyMudTrack`（载具演示）
- `Content/Input/`：Enhanced Input 资产；本仓新增玩法按键走 `Config/DefaultInput.ini` 传统 Action 映射
