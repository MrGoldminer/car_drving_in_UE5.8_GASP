# UI — 界面模块

C++ Slate 血条 HUD，无需 UMG 蓝图资产。

## 类结构

| 类 | 职责 |
| --- | --- |
| `UGameplayHUDWidget` | `UUserWidget` 子类，`RebuildWidget` 用纯 Slate 构建；由 `Character/GameplaySubsystem` 为本地玩家创建并 `AddToViewport` |

## 血条布局

| 位置 | 内容 | 样式 |
| --- | --- | --- |
| 屏幕底部居中 | 玩家血量（读取 Pawn 的 `UHealthComponent`） | 400×22，绿 → 黄（<50%）→ 红（<25%），含 `HP 当前/上限` 文本 |
| 屏幕顶端居中 | 准星指向的敌人血量（`ECC_Pawn` 射线，目标丢失后保留 3 秒） | 500×20 红色，含敌人名称与数值；无目标或目标死亡时隐藏 |

玩家上车后 Pawn 无 `UHealthComponent`，底部血条自动清空；切回人物恢复。

## 扩展

- 改样式：直接改 `RebuildWidget` 中的尺寸、颜色、边距。
- 加其他 HUD 元素（弹药数、武器名）：在 `NativeTick` 读取 `UWeaponManagerComponent` 后更新 Slate 控件。
