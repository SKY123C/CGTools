import cgtw2
import fbx
import FbxCommon
import os
import unreal

class AssetSign:
    
    @classmethod
    def is_camera(cls, src_file):
        return "_cam" in src_file.lower() and src_file.lower().endswith(".fbx")


class TWInterface:
    
    def __new__(cls, *args, **kwargs):
        tw = None
        try:
            tw = cgtw2.tw()
        except Exception as e:
            print(str(e))
        obj = super().__new__(cls)
        if tw:
            obj.tw = tw
            return obj
        else:
            return None
            
    def get_filebox_info(self, db_entity, module, filter, sign):
        all_db_info = self.get_all_project(db_entity, True)
        db = all_db_info.get("db")
        id_list = self.tw.task.get_id(db=db, module=module, filter_list=filter)
        result = self.tw.task.get_version_file(db, module, id_list, sign)
        return result

    def get_all_project(self, db_entity=None, bGet=False):
        t_id_list =  self.tw.info.get_id(db='public', module='project', filter_list=[])
        info_list = self.tw.info.get(db='public', module='project', id_list=t_id_list, field_sign_list=['project.entity','project.database'], order_sign_list=["project.entity"])
        result = {"info_list": info_list}
        if bGet and db_entity is not None:
            db = [i.get('project.database') for i in info_list if i.get('project.entity') == db_entity]
            result["db"] = db[0] if db else None
        return result
    
    # def get_all_tw_abc_file(self, db_entity, module, filter, sign, only_file=True):
    #     all_db_info = self.get_all_project(db_entity, True)
    #     db = all_db_info.get("db")
    #     id_list = self.tw.task.get_id(db, module, filter)
    #     if not id_list or len(id_list) > 1:
    #         return
    #     filebox_info = self.tw.task.get_sign_filebox(db, module, id_list[0], sign)
    #     print(filebox_info)
    #     version_info_list = filebox_info.get("filebox_info")
    #     new_version_info_list = sorted(version_info_list.items(), key=lambda x: x[1], reverse=True)
    #     for verion_id, version in new_version_info_list:
            
    #         ...
        
    #     max_version_file_idlist= self.tw.file.get_id(db=db, filter_list=[['#module','=',module], 'and',
    #                                                             ['#version_id','=',max_version_id]])
    #     file_data_list = self.tw.file.get(db=db, id_list=max_version_file_idlist,
    #                                 field_list=["sys_local_full_path"])
    #     file_list = [i.get('sys_local_full_path') for i in file_data_list if i.get('sys_local_full_path').lower().endswith(".abc")]
    #     if only_file:
    #         return file_list
    #     else:
    #         result = {"file_list": file_list, "version": filebox_info.get("max_version")}
    #         return result

    @classmethod
    def add_version_tag(cls, actor_list, file_path):
        if not actor_list:
            return
        actor = actor_list[0]
        tags = actor.get_editor_property("tags")
        if tags:
            new_tag = [tags[0], unreal.Name(file_path)]
            actor.set_editor_property("tags", new_tag)
    
    @classmethod
    def create_animation_task(cls, file_path, dst_path, skeleton_path):
        if unreal.SystemLibrary.get_engine_version().startswith("5"):
            asset_system = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
        else:
            asset_system = unreal.EditorAssetLibrary
        if not asset_system.does_asset_exist(skeleton_path):
            print(skeleton_path + ": 骨骼网格体不存在")
            return
        skele_obj = unreal.load_asset(skeleton_path)
        if skele_obj.get_class() == unreal.SkeletalMesh.static_class():
            skeleton = skele_obj.get_editor_property("skeleton")
        elif skele_obj.get_class() == unreal.Skeleton.static_class():
            skeleton = skele_obj
        else:
            skeleton = None
        if not skeleton:
            print(skeleton_path + ": 骨骼不存在")
            return
        import_task = unreal.AssetImportTask()
        options = unreal.FbxImportUI()
        import_task.replace_existing = True
        import_task.options = options
        import_task.save = True
        import_task.automated = True
        import_task.filename = file_path
        import_task.destination_path = dst_path
        '''
        FbxImportUI配置
        '''
        options.import_animations = True
        options.import_materials = False
        options.import_textures = False
        options.skeleton = skeleton
        options.anim_sequence_import_data.import_translation = unreal.Vector(0.0, 0.0, 0.0)
        options.anim_sequence_import_data.import_rotation = unreal.Rotator(0.0, 0.0, 0.0)
        options.anim_sequence_import_data.import_uniform_scale = 1.0
        # FbxAnimSequenceImportData
        options.anim_sequence_import_data.set_editor_property('animation_length', unreal.FBXAnimationLengthImportType.FBXALIT_EXPORTED_TIME)
        options.anim_sequence_import_data.set_editor_property('remove_redundant_keys', True)
        return import_task

    @classmethod
    def set_camera_property(cls, fbx_file_path, component):
        lSdkManager, lScene = FbxCommon.InitializeSdkObjects()
        lResult = FbxCommon.LoadScene(lSdkManager, lScene, fbx_file_path)
        root_node = lScene.GetRootNode()
        if not os.path.exists(fbx_file_path):
            return
        if not isinstance(component, unreal.CineCameraComponent):
            return 
        if not root_node:
            return
        for i in range(root_node.GetChildCount()):
            child_node = root_node.GetChild(i)
            node_type = child_node.GetNodeAttribute().GetAttributeType()
            if node_type == fbx.FbxNodeAttribute.eCamera:
                camera_attr_obj = child_node.GetNodeAttribute()
                if camera_attr_obj.GetApertureMode() == fbx.FbxCamera.eFocalLength:
                    FocalLength = camera_attr_obj.FocalLength.Get()
                    FieldOfView = camera_attr_obj.ComputeFieldOfView(FocalLength)
                else:
                    FieldOfView = camera_attr_obj.FieldOfView.Get()
                    FocalLength = camera_attr_obj.ComputeFocalLength(FieldOfView)
                ApertureWidth = camera_attr_obj.GetApertureWidth()
                ApertureHeight = camera_attr_obj.GetApertureHeight()
                component.set_projection_mode(unreal.CameraProjectionMode.PERSPECTIVE if camera_attr_obj.ProjectionType.Get() == fbx.FbxCamera.ePerspective else unreal.CameraProjectionMode.ORTHOGRAPHIC)
                component.set_aspect_ratio(camera_attr_obj.AspectWidth.Get() / camera_attr_obj.AspectHeight.Get())
                component.set_ortho_near_clip_plane(camera_attr_obj.NearPlane.Get())
                component.set_ortho_far_clip_plane(camera_attr_obj.FarPlane.Get())
                component.set_ortho_width(camera_attr_obj.OrthoZoom.Get())
                component.set_field_of_view(FieldOfView)
                if unreal.SystemLibrary.get_engine_version().startswith("4.27"):
                    component.filmback.sensor_width = 36.0
                    component.filmback.sensor_height = 20.25
                    component.focus_settings.focus_method = unreal.CameraFocusMethod.DISABLE
                else:
                    component.filmback_settings.sensor_width = ApertureWidth * 25.4
                    component.filmback_settings.sensor_height = ApertureHeight * 25.4
                if FocalLength < component.lens_settings.min_focal_length:
            
                    component.lens_settings.min_focal_length = FocalLength
            
                if FocalLength > component.lens_settings.max_focal_length:
            
                    component.lens_settings.max_focal_length = FocalLength
            
                component.current_focal_length = FocalLength

    @classmethod
    def create_sequence_camera(cls, camera_actor, camera_path, sequence):
        if not isinstance(camera_actor, unreal.CineCameraActor):
            return
        cls.set_camera_property(camera_path, camera_actor.camera_component)
        actor_binding_proxy = sequence.add_spawnable_from_instance(camera_actor)
        component_binding_proxy = sequence.add_possessable(camera_actor.camera_component)
        temp_proxy = component_binding_proxy.get_parent()
        component_binding_proxy.set_parent(actor_binding_proxy)
        temp_proxy.remove()
        unreal.EditorLevelLibrary.destroy_actor(camera_actor)
        return actor_binding_proxy
    
    @classmethod
    def import_camera_fbx(cls, cameraPath, sequence):
        tw_utl.
        engine_version = unreal.SystemLibrary.get_engine_version() #str
        if engine_version.startswith('4.21'):
            import_setting = unreal.MovieSceneUserImportFBXSettings()
            import_setting.set_editor_property('create_cameras', False)
            import_setting.set_editor_property('force_front_x_axis', False)
            import_setting.set_editor_property('match_by_name_only', False)
            import_setting.set_editor_property('reduce_keys', False)
            import_setting.set_editor_property('reduce_keys_tolerance', 0.001)
        elif engine_version.startswith('4.27'):
            import_setting = unreal.MovieSceneUserImportFBXSettings()
            import_setting.set_editor_property('create_cameras', False)
            import_setting.set_editor_property('force_front_x_axis', False)
            import_setting.set_editor_property('match_by_name_only', False)
            import_setting.set_editor_property('reduce_keys', False)
            import_setting.set_editor_property('reduce_keys_tolerance', 0.001)
            import_setting.set_editor_property('convert_scene_unit', False)
            import_setting.set_editor_property('import_uniform_scale', 1.0)
            import_setting.set_editor_property('replace_transform_track', False)
        else: #UE5
            import_setting = unreal.MovieSceneUserImportFBXSettings()
            import_setting.set_editor_property('create_cameras', False)
            import_setting.set_editor_property('force_front_x_axis', False)
            import_setting.set_editor_property('match_by_name_only', False)
            import_setting.set_editor_property('reduce_keys', False)
            import_setting.set_editor_property('reduce_keys_tolerance', 0.001)


        binding_proxy_list = [i for i in sequence.get_spawnables() if isinstance(i.get_object_template(), unreal.CineCameraActor)]
        if not binding_proxy_list:
            return
        for binding_proxy in binding_proxy_list:
            actors = cls.get_bound_object(sequence, binding_proxy, unreal.CameraComponent.static_class())
            if actors:
                cls.set_camera_property(cameraPath, actors[0].get_editor_property("camera_component"))
                cls.add_version_tag(cls.get_bound_object(sequence, binding_proxy, unreal.CineCameraActor.static_class()), cameraPath)
        world = unreal.EditorLevelLibrary.get_editor_world()
        if unreal.SystemLibrary.get_engine_version().startswith("4.21"):
            unreal.PythonSequenceLib.import_level_sequence_fbx(world, sequence, binding_proxy_list, import_setting, cameraPath)
        else:
            unreal.SequencerTools.import_level_sequence_fbx(world, sequence, binding_proxy_list, import_setting, cameraPath)
        # save the shot levelSequence
        unreal.CGToolsSequenceLib.update_sequence(sequence)
        unreal.EditorAssetLibrary.save_asset(sequence.get_path_name())
    
    @classmethod
    def get_bound_object(cls, sequence, binding_proxy, object_type):
        '''
        只支持spawnable类型
        '''
        bOpen = unreal.CGToolsSequenceLib.check_is_open(sequence)
        if bOpen:
            bound_objects = unreal.CGToolsSequenceLib.get_bound_objects(sequence, binding_proxy.get_id())
        else:
            id = binding_proxy.get_id().to_string()
            bound_objects = []
            for proxy in sequence.get_bindings():
                if proxy.get_id().to_string() == id and proxy.get_object_template():
                    bound_objects.append(proxy.get_object_template())
        return bound_objects
        