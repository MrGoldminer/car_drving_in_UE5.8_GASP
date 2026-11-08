import unreal

ROOT = "F:/Unreal Projects/GameAnimationSample/RawAssets/Weapons"
DEST = "/Game/Weapons/Models"


def import_model(src, name):
    task = unreal.AssetImportTask()
    task.filename = src
    task.destination_path = DEST
    task.destination_name = name
    task.automated = True
    task.replace_existing = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    asset_path = f"{DEST}/{name}.{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        mesh = unreal.EditorAssetLibrary.load_asset(asset_path)
        if isinstance(mesh, unreal.StaticMesh):
            bounds = mesh.get_bounds()
            unreal.log(f"[Weapons] {name} bounds min={bounds.box_min} max={bounds.box_max}")
            for i in range(mesh.get_num_materials()):
                unreal.log(f"[Weapons] {name} material slot {i}: {mesh.get_material(i).get_path_name()}")
        unreal.log(f"[Weapons] Imported {name} -> {asset_path}")
    else:
        unreal.log_error(f"[Weapons] Import failed for {name}")


import_model(f"{ROOT}/Katana.glb", "Katana")
import_model(f"{ROOT}/TangDao.glb", "TangDao")
import_model(f"{ROOT}/AK47.glb", "AK47")

unreal.EditorAssetLibrary.save_directory(DEST)
unreal.log("[Weapons] Import finished")
