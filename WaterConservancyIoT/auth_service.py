import jwt
import bcrypt
import logging
from datetime import datetime, timedelta, timezone
from typing import Dict, Any, Optional

from mysql_client import MySQLClient
from mysql.connector import Error

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# !!! 警告: 这是一个非常重要的密钥，在生产环境中必须通过安全的方式（如环境变量）管理 !!!
# !!! 绝不能硬编码在代码中。此处仅为演示目的。!!!
JWT_SECRET_KEY = "a_very_secret_and_secure_key_that_should_be_changed"
JWT_ALGORITHM = "HS256"
JWT_EXPIRATION_MINUTES = 60

class AuthService:
    """
    负责处理用户认证、密码管理和JWT生成/验证的服务。
    """
    def __init__(self, secret_key: str, mysql_client: MySQLClient):
        self.secret_key = secret_key
        self.mysql_client = mysql_client

    def hash_password(self, password: str) -> str:
        """使用bcrypt对明文密码进行哈希处理。"""
        salt = bcrypt.gensalt()
        hashed = bcrypt.hashpw(password.encode('utf-8'), salt)
        return hashed.decode('utf-8')

    def verify_password(self, plain_password: str, hashed_password: str) -> bool:
        """验证明文密码是否与哈希密码匹配。"""
        return bcrypt.checkpw(plain_password.encode('utf-8'), hashed_password.encode('utf-8'))

    def create_jwt_token(self, user_id: int) -> str:
        """生成JWT令牌"""
        try:
            payload = {
                'sub': str(user_id),  # 修正: 'sub' 必须是字符串
                'iat': datetime.utcnow(),
                'exp': datetime.utcnow() + timedelta(days=1)
            }
            token = jwt.encode(payload, self.secret_key, algorithm='HS256')
            return token
        except Exception as e:
            print(f"创建JWT时出错: {e}")
            return None

    def decode_jwt_token(self, token: str):
        """解码并验证JWT令牌"""
        try:
            payload = jwt.decode(token, self.secret_key, algorithms=['HS256'])
            return payload
        except jwt.ExpiredSignatureError:
            print("Token已过期")
            return None
        except jwt.InvalidTokenError as e:
            print(f"无效的Token: {e}")
            return None

    def register_user(self, username: str, password: str, role: str = 'viewer'):
        """注册新用户"""
        hashed_password = self.hash_password(password)
        return self.mysql_client.create_user(username, hashed_password, role)

    def authenticate_user(self, username, password):
        """验证用户凭据"""
        user_data = self.mysql_client.get_user_by_username(username)
        if user_data and self.verify_password(password, user_data['password_hash']):
            return {'id': user_data['id'], 'username': user_data['username']}
        return None

def initialize_default_admin(auth_service: AuthService):
    """如果admin用户不存在，则创建它"""
    if not auth_service.mysql_client.get_user_by_username("admin"):
        print("未找到admin用户，正在创建默认管理员...")
        success = auth_service.register_user("admin", "password123", "admin")
        if success:
            print("✅ 成功创建默认管理员(admin/password123)。")
        else:
            print("❌ 创建默认管理员失败。")
    else:
        print("Admin用户已存在。")


if __name__ == '__main__':
    # 允许直接运行此文件来创建默认管理员
    # 注意: 这里需要一个MySQLClient实例来调用initialize_default_admin
    # 在实际应用中，MySQLClient应该通过依赖注入或工厂模式获取
    # 为了简化，这里直接创建一个临时的MySQLClient
    try:
        mysql_cli = MySQLClient(
            host='localhost',
            database='sensordatabase',
            user='root',
            password='123456'
        )
        auth_service = AuthService(JWT_SECRET_KEY, mysql_cli)
        initialize_default_admin(auth_service)
        mysql_cli.close()
    except Exception as e:
        print(f"初始化失败: {e}")
