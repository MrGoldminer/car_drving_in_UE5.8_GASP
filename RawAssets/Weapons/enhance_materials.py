import unreal

DEST = "/Game/Weapons/Models"


def set_pbr(material, metallic, roughness):
    if not isinstance(material, unreal.Material):
        return

    def connect_constant(value, prop):
        expr = unreal.MaterialEditingLibrary.create_material_expression(
            material, unreal.MaterialExpressionConstant, 0, 0)
        expr.set_editor_property("r", value)
        unreal.MaterialEditingLibrary.connect_material_property(expr, "", prop)

    connect_constant(metallic, unreal.MaterialProperty.MP_METALLIC)
    connect_constant(roughness, unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.EditorAssetLibrary.save_loaded_asset(material)
    unreal.log(f"[Weapons] PBR set on {material.get_path_name()}")


def enhance(path, metallic, roughness):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        unreal.log_warning(f"[Weapons] Missing material {path}")
        return
    if isinstance(asset, unreal.MaterialInstanceConstant):
        asset = asset.parent
    set_pbr(asset, metallic, roughness)


enhance(f"{DEST}/TangDao/Materials/Steel.Steel", 0.85, 0.3)
enhance(f"{DEST}/TangDao/Materials/LightSteel.LightSteel", 0.85, 0.25)
enhance(f"{DEST}/TangDao/Materials/DarkSteel.DarkSteel", 0.8, 0.4)
enhance(f"{DEST}/TangDao/Materials/DarkWood.DarkWood", 0.0, 0.65)
enhance(f"{DEST}/TangDao/Materials/LightWood.LightWood", 0.0, 0.6)
enhance(f"{DEST}/AK47/Materials/Grey.Grey", 0.8, 0.4)
enhance(f"{DEST}/AK47/Materials/Grey2.Grey2", 0.8, 0.35)
enhance(f"{DEST}/AK47/Materials/DarkGrey.DarkGrey", 0.75, 0.45)
enhance(f"{DEST}/AK47/Materials/Wood.Wood", 0.0, 0.6)
enhance(f"{DEST}/Katana/Materials/lambert1.lambert1", 0.85, 0.3)

unreal.log("[Weapons] Material enhancement finished")
