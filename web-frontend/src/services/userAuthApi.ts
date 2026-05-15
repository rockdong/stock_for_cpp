import axios from 'axios';

const API_BASE_URL = '/api';

export interface RegisterRequest {
  username: string;
  password: string;
}

export interface LoginRequest {
  username: string;
  password: string;
}

export interface AuthResponse {
  success: boolean;
  data?: {
    token: string;
    user: {
      id: number;
      username: string;
      role: string;
    };
  };
  message?: string;
}

export interface VerifyResponse {
  valid: boolean;
  user?: {
    sub: number;
    username: string;
    role: string;
  };
  error?: string;
}

export const userAuthApi = {
  register: async (data: RegisterRequest): Promise<AuthResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/register`, data);
    return response.data;
  },

  login: async (data: LoginRequest): Promise<AuthResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/login`, data);
    return response.data;
  },

  verify: async (token: string): Promise<VerifyResponse> => {
    const response = await axios.post(`${API_BASE_URL}/auth/verify`, { token });
    return response.data;
  },
};