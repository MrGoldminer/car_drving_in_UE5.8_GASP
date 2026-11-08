import unreal

PATH = "/Game/Weapons/Materials"
NAME = "M_BladeTrail"

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mat = asset_tools.create_asset(NAME, PATH, unreal.Material, unreal.MaterialFactoryNew())

mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
mat.set_editor_property("two_sided", True)

vertex_color = unreal.MaterialEditingLibrary.create_material_expression(
    mat, unreal.MaterialExpressionVertexColor, 0, 0)
unreal.MaterialEditingLibrary.connect_material_property(
    vertex_color, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
unreal.MaterialEditingLibrary.connect_material_property(
    vertex_color, "A", unreal.MaterialProperty.MP_OPACITY)

unreal.EditorAssetLibrary.save_loaded_asset(mat)
unreal.log(f"[Weapons] Trail material created: {mat.get_path_name()}")
