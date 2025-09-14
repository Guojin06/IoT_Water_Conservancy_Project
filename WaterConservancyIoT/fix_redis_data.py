#!/usr/bin/env python3
"""
Redis数据修复脚本 - 清理错误的数据类型
"""

import redis
import json

def clear_redis_sensor_data():
    """清理Redis中的传感器数据"""
    try:
        r = redis.Redis(host='localhost', port=6379, decode_responses=True)
        
        # 检查连接
        if not r.ping():
            print("❌ 无法连接到Redis服务器")
            return False
        
        print("✅ 已连接到Redis服务器")
        
        # 清理所有传感器相关的键
        print("🧹 清理传感器数据...")
        
        patterns_to_delete = [
            "sensor:*:latest",
            "sensor:*:updates", 
            "statistics:*",
            "sensor_data:*"
        ]
        
        total_deleted = 0
        for pattern in patterns_to_delete:
            keys = r.keys(pattern)
            if keys:
                deleted = r.delete(*keys)
                total_deleted += deleted
                print(f"  删除了 {deleted} 个键匹配模式 '{pattern}'")
            else:
                print(f"  没有找到匹配模式 '{pattern}' 的键")
        
        print(f"✅ 总共删除了 {total_deleted} 个键")
        
        # 验证清理结果
        remaining_sensor_keys = r.keys("sensor:*")
        if remaining_sensor_keys:
            print(f"⚠️  仍有 {len(remaining_sensor_keys)} 个传感器相关键未清理")
        else:
            print("✅ 所有传感器数据已清理完毕")
        
        return True
        
    except Exception as e:
        print(f"❌ Redis清理过程中出错: {e}")
        return False

if __name__ == "__main__":
    print("🚀 Redis数据修复工具启动...")
    print("=" * 50)
    
    if clear_redis_sensor_data():
        print("\n✅ Redis数据清理完成")
        print("💡 现在可以重启 data_producer.py 来生成新的数据")
    else:
        print("\n❌ Redis数据清理失败")
