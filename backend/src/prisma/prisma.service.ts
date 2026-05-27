import { Injectable, OnModuleInit, OnModuleDestroy } from '@nestjs/common';
import { PrismaClient } from '@prisma/client';

@Injectable()
export class PrismaService extends PrismaClient implements OnModuleInit, OnModuleDestroy {
  private retryAttempts = 5;
  private retryDelayMs = 3000;
  
  async onModuleInit() {
    await this.connectWithRetry();
  }

  async onModuleDestroy() {
    await this.$disconnect();
    console.log('Prisma disconnected from database');
  }

  private async connectWithRetry() {
    for (let attempt = 1; attempt <= this.retryAttempts; attempt++) {
      try {
        await this.$connect();
        console.log('Prisma connected to database successfully');
        return;
      } catch (error) {
        console.error(`Database connection attempt ${attempt}/${this.retryAttempts} failed:`, error.message);
        
        if (attempt === this.retryAttempts) {
          console.error('Failed to connect to database after all retries');
          throw error;
        }
        
        await new Promise(resolve => setTimeout(resolve, this.retryDelayMs));
      }
    }
  }

  async healthCheck(): Promise<boolean> {
    try {
      await this.$queryRaw`SELECT 1`;
      return true;
    } catch (error) {
      console.error('Database health check failed:', error.message);
      return false;
    }
  }

  async reconnect() {
    try {
      await this.$disconnect();
      await this.connectWithRetry();
      return true;
    } catch (error) {
      console.error('Database reconnect failed:', error.message);
      return false;
    }
  }
}