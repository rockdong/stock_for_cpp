const TOKEN_KEY = 'auth_token';

export const tokenStorage = {
  save: (token: string) => {
    localStorage.setItem(TOKEN_KEY, token);
  },

  get: (): string | null => {
    return localStorage.getItem(TOKEN_KEY);
  },

  remove: () => {
    localStorage.removeItem(TOKEN_KEY);
  },

  exists: (): boolean => {
    return !!localStorage.getItem(TOKEN_KEY);
  }
};