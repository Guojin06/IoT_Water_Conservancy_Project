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
    def __init__(self, mysql_client: MySQLClient):
        self.mysql = mysql_client

    def hash_password(self, plain_password: str) -> str:
        """使用bcrypt对明文密码进行哈希处理。"""
        salt = bcrypt.gensalt()
        hashed_password = bcrypt.hashpw(plain_password.encode('utf-8'), salt)
        return hashed_password.decode('utf-8')

    def verify_password(self, plain_password: str, hashed_password: str) -> bool:
        """验证明文密码是否与哈希密码匹配。"""
        return bcrypt.checkpw(plain_password.encode('utf-8'), hashed_password.encode('utf-8'))

    def create_jwt_token(self, user_id: int, username: str, role: str) -> str:
        """为指定用户生成一个JWT。"""
        payload = {
            "sub": user_id,  # 'subject', 通常是用户ID
            "name": username,
            "role": role,
            "iat": datetime.now(timezone.utc),  # 'issued at', 签发时间
            "exp": datetime.now(timezone.utc) + timedelta(minutes=JWT_EXPIRATION_MINUTES)  # 'expiration time', 过期时间
        }
        token = jwt.encode(payload, JWT_SECRET_KEY, algorithm=JWT_ALGORITHM)
        return token

    def decode_jwt_token(self, token: str) -> Optional[Dict[str, Any]]:
        """解码并验证一个JWT。如果无效或过期，则返回None。"""
        try:
            payload = jwt.decode(token, JWT_SECRET_KEY, algorithms=[JWT_ALGORITHM])
            return payload
        except jwt.ExpiredSignatureError:
            logger.warning("Token已过期。")
            return None
        except jwt.InvalidTokenError as e:
            logger.error(f"无效的Token: {e}")
            return None

    def register_user(self, username: str, plain_password: str, role: str = 'viewer') -> bool:
        """注册一个新用户，密码将被哈希处理。"""
        if not self.mysql.is_connected():
            return False
        
        password_hash = self.hash_password(plain_password)
        query = "INSERT INTO users (username, password_hash, role) VALUES (%s, %s, %s)"
        params = (username, password_hash, role)
        
        success = self.mysql.execute_query(query, params)
        if success:
            logger.info(f"用户 '{username}' 注册成功。")
        else:
            logger.error(f"用户 '{username}' 注册失败（可能已存在）。")
        return success

    def authenticate_user(self, username: str, plain_password: str) -> Optional[str]:
        """
        验证用户凭证。如果成功，返回生成的JWT；否则返回None。
        """
        if not self.mysql.is_connected():
            return None
            
        cursor = None
        try:
            cursor = self.mysql.connection.cursor(dictionary=True)
            cursor.execute("SELECT * FROM users WHERE username = %s", (username,))
            user = cursor.fetchone()
            
            if user and self.verify_password(plain_password, user['password_hash']):
                logger.info(f"用户 '{username}' 认证成功。")
                return self.create_jwt_token(user['user_id'], user['username'], user['role'])
            else:
                logger.warning(f"用户 '{username}' 认证失败（用户名或密码错误）。")
                return None
        except Error as e:
            logger.error(f"认证过程中数据库查询出错: {e}")
            return None
        finally:
            if cursor:
                cursor.close()

def initialize_default_admin():
    """
    在数据库中创建一个默认的管理员账户，方便首次启动和测试。
    """
    logger.info("--- 正在初始化默认管理员账户 ---")
    
    mysql_cli = MySQLClient(
        host='localhost',
        database='sensordatabase',
        user='root',
        password='123456'
    )
    
    if not mysql_cli.is_connected():
        logger.critical("无法连接到数据库，无法创建默认管理员。")
        return

    auth_service = AuthService(mysql_cli)
    
    # 这里我们只在用户不存在时创建，避免重复
    # 实际应用中可以用更优雅的方式检查用户是否存在
    logger.info("正在创建默认管理员 'admin' (密码: password123)...")
    auth_service.register_user(
        username='admin',
        plain_password='password123',
        role='admin'
    )
    
    mysql_cli.close()
    logger.info("--- 初始化完成 ---")


if __name__ == '__main__':
    # 允许直接运行此文件来创建默认管理员
    initialize_default_admin()
