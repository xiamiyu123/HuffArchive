#pragma once

namespace Util {

class SystemUtils {
public:
    /**
     * @brief 注册 .huff 文件关联
     * @return true 如果成功，false 否则
     */
    static bool registerFileAssociation();
    
    /**
     * @brief 检查是否已关联
     */
    static bool isFileAssociationRegistered();
};

}
