#!/usr/bin/env python3
"""
数据流调试脚本 - 检查Redis中的数据和服务状态
"""

import redis
import json
import time
from datetime import datetime

def check_redis_connection():
    """检查Redis连接"""
    try:
        r = redis.Redis(host='localhost', port=6379, decode_responses=True)
        pong = r.ping()
        if pong:
            print("✅ Redis服务器连接正常")
            return r
        else:
            print("❌ Redis服务器连接失败")
            return None
    except Exception as e:
        print(f"❌ Redis连接错误: {e}")
        return None

def check_redis_data(r):
    """检查Redis中的数据"""
    print("\n🔍 检查Redis中的传感器数据...")
    
    # 检查所有传感器键
    sensor_keys = r.keys("sensor:*:latest")
    if not sensor_keys:
        print("❌ Redis中没有找到任何传感器数据")
        return
    
    print(f"📊 找到 {len(sensor_keys)} 个传感器的最新数据:")
    
    for key in sorted(sensor_keys):
        try:
            # 修正: 使用 HGETALL 读取哈希数据
            data = r.hgetall(key)
            if data:
                timestamp = data.get('timestamp', 'N/A')
                value = data.get('value', 'N/A')
                print(f"  {key}: value={value}, timestamp={timestamp}")
            else:
                print(f"  {key}: 无数据")
        except Exception as e:
            print(f"  {key}: 解析错误 - {e}")

def check_statistics_data(r):
    """检查统计数据"""
    print("\n📈 检查统计数据...")
    
    try:
        stats = r.get("statistics:latest")
        if stats:
            stats_data = json.loads(stats)
            print(f"  统计数据: {stats_data}")
        else:
            print("  ❌ 没有找到统计数据")
    except Exception as e:
        print(f"  ❌ 统计数据解析错误: {e}")

def monitor_data_updates(r, duration=10):
    """监控数据更新（持续几秒钟）"""
    print(f"\n🔄 监控数据更新 ({duration}秒)...")
    
    # 获取初始时间戳
    initial_timestamps = {}
    sensor_keys = r.keys("sensor:*:latest")
    
    for key in sensor_keys:
        try:
            data = r.get(key)
            if data:
                sensor_data = json.loads(data)
                initial_timestamps[key] = sensor_data.get('timestamp')
        except:
            pass
    
    time.sleep(duration)
    
    # 检查是否有更新
    updated_count = 0
    for key in sensor_keys:
        try:
            # 修正: 使用 HGETALL 读取哈希数据
            data = r.hgetall(key)
            if data:
                current_timestamp = data.get('timestamp')
                if current_timestamp != initial_timestamps.get(key):
                    updated_count += 1
                    print(f"  📝 {key} 数据已更新")
        except:
            pass
    
    if updated_count == 0:
        print("  ❌ 在监控期间没有检测到数据更新")
        print("  💡 这可能说明 data_producer.py 没有正常工作")
    else:
        print(f"  ✅ 检测到 {updated_count} 个传感器的数据更新")

if __name__ == "__main__":
    print("🚀 数据流调试工具启动...")
    print("=" * 50)
    
    # 检查Redis连接
    r = check_redis_connection()
    if not r:
        exit(1)
    
    # 检查当前数据
    check_redis_data(r)
    check_statistics_data(r)
    
    # 监控数据更新
    monitor_data_updates(r, 10)
    
    print("\n✅ 调试完成")
