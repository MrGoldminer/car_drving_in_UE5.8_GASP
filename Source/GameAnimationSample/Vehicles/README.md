# Vehicles — 载具系统模块

可驾驶载具：自定义悬挂物理、上下车、自动泊车、泥地赛道与水花效果。

## 类结构

| 类 | 职责 |
| --- | --- |
| `ABumperCarPawn` | 载具物理基类：盒体车身碰撞、四轮悬挂（弹簧/阻尼）、油门转向刹车输入、车轮状态（`FBumperCarWheelState`）、重置 |
| `ABuggyPawn` | Buggy 载具：程序化车身/车轮模型（基础几何体）、`EnterVehicle/ExitVehicle` 上下车、驾驶员座位与动画、水花粒子（实例化网格）、`ToggleAutoPark` 自动泊车 |
| `AParkingSpot` | 泊车位标记，供自动泊车搜索与对位 |
| `AMudTrackSegment` | 泥地赛道路段，配合 Buggy 演示 |

## 输入（Config/DefaultInput.ini）

| Action / Axis | 键 | 行为 |
| --- | --- | --- |
| `BumperCar_Throttle` | W / S | 油门 / 倒车 |
| `BumperCar_Steering` | A / D | 转向 |
| `BumperCar_Brake` | Space | 手刹 |
| `BumperCar_Reset` | R | 重置载具 |
| `VehicleInteract` | F | 视角对准载具进入；驾驶中离开 |
| `BumperCar_AutoPark` | P | 自动泊车开关 |

交互由 `Character/VehicleInteractionComponent`（相机射线检测）触发；人物上车后输入移转载具。

## 模型

当前 Buggy 使用引擎 Cube/Cylinder 基础几何体拼装（简化占位版本），`WheelRadius/WheelBase/TrackWidth` 等参数与物理匹配。正式美术资产可在 `BuggyPawn` 构造函数中替换。

## 演示关卡

`Content/Levels/BuggyMudTrack`：泥地赛道 + 泊车位。默认启动关卡为 `DefaultLevel`（可在 `Config/DefaultEngine.ini` 调整）。
