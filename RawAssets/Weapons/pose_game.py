import unreal

world = unreal.EditorLevelLibrary.get_editor_world()
pc = unreal.GameplayStatics.get_player_controller(world, 0)
pawn = pc.get_pawn()
if pawn:
    pawn.set_actor_location(unreal.Vector(0.0, 0.0, 100.0), False, False)
    pc.set_control_rotation(unreal.Rotator(0.0, 0.0, 0.0))

models = [
    ("Katana", 0.1376, (90.0, 0.0, 0.0), (200.0, -150.0, 100.0)),
    ("TangDao", 0.456, (90.0, 0.0, 0.0), (200.0, 0.0, 100.0)),
    ("AK47", 0.633, (0.0, 0.0, 0.0), (200.0, 150.0, 100.0)),
]

for name, scale, rot, loc in models:
    mesh = unreal.EditorAssetLibrary.load_asset(
        f"/Game/Weapons/Models/{name}/StaticMeshes/{name}.{name}")
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor, unreal.Vector(*loc), unreal.Rotator(*rot))
    actor.set_actor_label(f"PosePreview_{name}")
    actor.static_mesh_component.set_static_mesh(mesh)
    actor.static_mesh_component.set_relative_scale3d(unreal.Vector(scale, scale, scale))
    unreal.log(f"[PosePreview] {name} placed at {loc}")
