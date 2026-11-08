import unreal

for name in ["Katana", "TangDao", "AK47"]:
    path = f"/Game/Weapons/Models/{name}/StaticMeshes/{name}.{name}"
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if mesh:
        bounds = mesh.get_bounding_box()
        unreal.log(f"[Weapons] {name} bounds min={bounds.min} max={bounds.max}")
    else:
        unreal.log_error(f"[Weapons] Missing {path}")
