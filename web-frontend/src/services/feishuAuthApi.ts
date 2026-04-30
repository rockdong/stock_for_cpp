import axios from 'axios';

const api = axios.create({
  baseURL: '/api',
  timeout: 30000,
});

export const feishuAuthApi = {
  getQRCode: async (): Promise<{ session_id: string; auth_url: string; qr_url: string; expires_in: number }> => {
    const response = await api.get('/auth/feishu/qrcode');
    return response.data.data;
  },

  getStatus: async (sessionId: string): Promise<{ status: string; user_id?: string; token?: string; is_expired: boolean }> => {
    const response = await api.get('/auth/feishu/status', { params: { session_id: sessionId } });
    return response.data.data;
  },
};