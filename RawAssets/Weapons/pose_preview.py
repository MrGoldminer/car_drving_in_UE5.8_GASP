import unreal

for actor in unreal.EditorLevelLibrary.get_all_level_actors():
    if actor.get_name().startswith("PosePreview"):
        unreal.EditorLevelLibrary.destroy_actor(actor)

models = [
    ("Katana", 0.1376, (90.0, 0.0, 0.0), (0.0, 0.0, 100.0)),
    ("TangDao", 0.456, (90.0, 0.0, 0.0), (150.0, 0.0, 100.0)),
    ("AK47", 0.633, (0.0, 0.0, 0.0), (300.0, 0.0, 100.0)),
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
