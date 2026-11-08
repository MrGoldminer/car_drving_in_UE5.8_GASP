# Weapons — 武器库模块

人物武器系统：武器生成、装备切换、近战与射击伤害、弹道轨迹表现。

## 类结构

| 类 | 职责 |
| --- | --- |
| `EWeaponType` | `None / Katana / TangDao / AK47` |
| `AWeaponBase` | 武器基类：零件拼装（`AddBoxPart/AddCylinderPart`）、可见性、伤害应用（`ApplyWeaponDamage`）、握持偏移（`GripLocation/GripRotation`） |
| `AMeleeWeapon` | 太刀 / 唐刀。`StartAttack` 触发挥砍：Tick 驱动武器挥动，激活窗口内做球形 Sweep，命中 Pawn 施加伤害；单次挥砍内同一目标只命中一次 |
| `ARifleWeapon` | AK47。按住攻击全自动射击（600 RPM）；相机射线判定命中（`ECC_Pawn` 多命中通道，兼容人物 capsule 的 Overlap 响应与墙体阻挡；不能用 `ECC_Visibility`，人物 capsule 默认忽略该通道），枪口到命中点生成弹道轨迹（tracer），附枪口火光 |
| `UWeaponManagerComponent` | 挂在人物上的武器库：BeginPlay 生成三把武器，`EquipWeapon` 挂到 `hand_r` socket，`CycleWeapon` 循环切换 |

## 输入（Config/DefaultInput.ini）

| Action | 键 | 行为 |
| --- | --- | --- |
| `WeaponCycle` | Tab | 徒手 → 太刀 → 唐刀 → AK47 → 徒手 |
| `WeaponAttack` | 鼠标左键 | 按下攻击，松开停止（AK47 连发） |

组件由 `Character/GameplaySubsystem` 自动挂载到玩家 Pawn，无需改人物蓝图。

## 参数

- 近战：`Damage=35`、`Reach=220cm`、`SwingDuration=0.4s`、`AttackCooldown=0.5s`
- AK47：`Damage=25`、`RoundsPerMinute=600`、`Range=10000cm`、`TracerLifetime=0.06s`

## 正式模型与材质

三把武器已接入带贴图/PBR 材质的正式模型（`Content/Weapons/Models/<名称>/StaticMeshes/`），由 `WeaponManagerComponent` 设置 `ModelAsset / ModelScale / ModelRotation`，加载失败时回退几何拼装。

| 武器 | 模型来源 | 材质 |
| --- | --- | --- |
| 太刀 | [Poly Pizza](https://poly.pizza/m/zV3WXbyjMf) | `lambert1` + 贴图，金属度 0.85 / 粗糙度 0.3 |
| 唐刀 | [Poly Pizza](https://poly.pizza/m/9lLmH8Et4K) | Steel 系金属 0.8~0.85，Wood 粗糙度 0.6~0.65 |
| AK47 | [Quaternius / Poly Pizza](https://poly.pizza/m/em1Hi9GuCv) | Grey 系金属 0.75~0.8，Wood 粗糙度 0.6 |

许可以各模型页面为准（Quaternius 为 CC0，Google Poly 存档通常为 CC-BY，建议保留署名）。导入/校准脚本在 `RawAssets/Weapons/`（`import_weapons.py`、`inspect_weapons.py`、`enhance_materials.py`）。

光影：模型使用 PBR 材质，配合关卡光照自然产生金属高光与明暗；`enhance_materials.py` 批量设置了金属度与粗糙度。

## 扩展

- **换正式模型**：替换 `BuildKatanaModel / BuildTangDaoModel / BuildModel` 中的几何拼装为 StaticMesh 资产；tracer 用 `TracerMaterial` 指定自发光材质。
- **加新武器**：`EWeaponType` 加枚举 → 新 `AWeaponBase` 子类 → `UWeaponManagerComponent` 生成并加入 `FindWeapon` / 切换循环。
- **接伤害系统**：命中走 `UGameplayStatics::ApplyDamage`，目标实现 `TakeDamage` / `OnTakeAnyDamage` 即可（如 AI 血量、物理反馈）。
