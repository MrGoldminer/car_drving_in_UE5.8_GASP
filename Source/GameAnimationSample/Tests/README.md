# Tests — 自动化测试

基于 Unreal Automation Framework 的行为测试，覆盖人物/载具切换与武器系统核心逻辑。

## 用例

| 测试 | 验证点 |
| --- | --- |
| `GameAnimationSample.Vehicles.EnterExitRestoresCharacter` | 上车后控制器在小车、人物 Tick/碰撞关闭；下车后控制器归还人物、状态全部恢复 |
| `GameAnimationSample.Vehicles.FSelectsNearestEmptyBuggy` | 多辆车时 F 只占用最近的空车，远处与超程车辆不受影响 |
| `GameAnimationSample.Weapons.TabCyclesAllWeapons` | Tab 按 徒手→太刀→唐刀→AK47→徒手 循环，装备武器可见并挂到人物 |
| `GameAnimationSample.Weapons.AK47ShotDamagesTarget` | AK47 开火对前方目标造成伤害（射线命中 + ApplyDamage） |
| `GameAnimationSample.Weapons.KatanaSwingDamagesTargetOnce` | 太刀一次挥砍对目标造成且仅造成一次伤害 |

`ATestDamageReceiver`：记录累计伤害的测试目标，用于验证近战/射击命中。

## 运行

```bash
UnrealEditor-Cmd.exe "<项目路径>.uproject" \
  -ExecCmds="Automation RunTests GameAnimationSample" \
  -unattended -NullRHI -nop4 -NoSplash -log
```

或在编辑器内：菜单 **Tools → Test Automation**，搜索 `GameAnimationSample` 勾选运行。

注意：测试需编入基础 DLL（完整编译），Live Coding patch 不会对独立测试进程生效。
