import axios from 'axios';

const api = axios.create({
  baseURL: '/api',
  timeout: 30000,
});

export const authApi = {
  getQRCode: async (): Promise<{ session_id: string; qr_url: string; expires_in: number }> => {
    const response = await api.get('/auth/qrcode');
    return response.data.data;
  },

  getStatus: async (sessionId: string): Promise<{ status: string; user_id?: number }> => {
    const response = await api.get('/auth/status', { params: { session_id: sessionId } });
    return response.data.data;
  },

  getToken: async (sessionId: string): Promise<{ token: string; expires_in: number }> => {
    const response = await api.post('/auth/token', { session_id: sessionId });
    return response.data.data;
  },
};